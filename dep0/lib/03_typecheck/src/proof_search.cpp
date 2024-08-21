#include "private/proof_search.hpp"

#include "private/tactics/search_app.hpp"
#include "private/tactics/search_trivial_value.hpp"
#include "private/tactics/search_true_t.hpp"
#include "private/tactics/search_var.hpp"

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/hash_code.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <unordered_map>

namespace dep0::typecheck {

struct search_state_t
{
    struct eq_t { bool operator()(expr_t const&, expr_t const&) const; };
    struct hash_t { std::size_t operator()(expr_t const&) const; };

    /**
     * Caches the result of previous searches, in case we need to search again for a value of the same type.
     * The key is the target type and the value is the result of a previous search.
     */
    using cache_t = std::unordered_map<expr_t, expr_t, hash_t, eq_t>;
    cache_t cache;

    std::chrono::steady_clock::time_point const deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);

    env_t const& env;
    ctx_t const& ctx;

    search_task_t main_task;

    search_state_t(
        env_t const& env,
        ctx_t const& ctx,
        expr_t const& target,
        ast::is_mutable_t const is_mutable_allowed,
        std::shared_ptr<usage_t> usage,
        ast::qty_t const usage_multiplier)
    :   env(env),
        ctx(ctx),
        main_task(
            *this,
            0ul,
            std::make_shared<expr_t>(target),
            is_mutable_allowed,
            std::move(usage),
            usage_multiplier,
            proof_search)
    { }

    bool expired() const { return std::chrono::steady_clock::now() > deadline; }
};

bool search_state_t::eq_t::operator()(expr_t const& x, expr_t const& y) const
{
    // TODO should this be beta-delta equivalence instead? needs a test
    return is_alpha_equivalent(x, y).has_value()
        and std::visit(
            boost::hana::overload(
                [] (expr_t const& x_type, expr_t const& y_type)
                {
                    return is_alpha_equivalent(x_type, y_type).has_value();
                },
                [] (kind_t, kind_t)
                {
                    return true;
                },
                [] <typename T, typename U> requires (not std::is_same_v<T, U>)
                (T const&, U const&)
                {
                    return false;
                }),
            x.properties.sort.get(), y.properties.sort.get());
}

std::size_t search_state_t::hash_t::operator()(expr_t const& x) const
{
    return hash_code(x);
}

search_task_t::search_task_t(
    search_state_t& st,
    std::size_t const depth,
    std::shared_ptr<expr_t const> target,
    ast::is_mutable_t const is_mutable_allowed,
    std::shared_ptr<usage_t> usage,
    ast::qty_t const usage_multiplier,
    std::function<void(search_task_t&)> task)
:   m_kind(one_t{std::move(task)}),
    state(st),
    depth(depth),
    env(st.env),
    ctx(st.ctx),
    target(std::move(target)),
    is_mutable_allowed(is_mutable_allowed),
    usage(std::move(usage)),
    usage_multiplier(usage_multiplier)
{ }

bool search_task_t::done() const { return not std::holds_alternative<in_progress_t>(m_status); }
bool search_task_t::failed() const { return std::holds_alternative<failed_t>(m_status); }
bool search_task_t::succeeded() const { return std::holds_alternative<succeeded_t>(m_status); }

expr_t& search_task_t::result()
{
    auto const succeeded = std::get_if<succeeded_t>(&m_status);
    assert(succeeded);
    return succeeded->result;
}

expr_t const& search_task_t::result() const
{
    auto const succeeded = std::get_if<succeeded_t>(&m_status);
    assert(succeeded);
    return succeeded->result;
}

void search_task_t::set_failed()
{
    assert(not done());
    m_status = failed_t{};
}

void search_task_t::set_result(expr_t x)
{
    assert(not done());
    state.cache.try_emplace(*target, x);
    m_status = succeeded_t{std::move(x)};
}

void search_task_t::run()
{
    assert(not done());
    if (done())
        return;
    if (depth > 10 or state.expired())
        return set_failed();
    match(
        m_kind,
        [this] (one_t& one)
        {
            one.tactic(*this);
            bool const implicitly_failed = not done() and std::holds_alternative<one_t>(m_kind);
            if (implicitly_failed)
                return set_failed();
        },
        [this] (any_t& any)
        {
            for (auto& t: any.sub_tasks)
            {
                if (not t.done())
                    t.run();
                if (t.succeeded())
                {
                    set_result(std::move(t.result()));
                    return;
                }
            }
            if (std::ranges::all_of(any.sub_tasks, [] (search_task_t const& t) { return t.failed(); }))
                set_failed();
        },
        [this] (all_t& all)
        {
            for (auto& t: all.sub_tasks)
                if (not t.done())
                {
                    t.run();
                    if (t.failed())
                        return set_failed(); // if any sub-tasks fails, the parent task automatically fails
                }
            if (std::ranges::all_of(all.sub_tasks, [] (search_task_t const& t) { return t.succeeded(); }))
            {
                std::vector<expr_t> results;
                results.reserve(all.sub_tasks.size());
                for (auto& t: all.sub_tasks)
                    results.push_back(std::move(t.result()));
                *usage += *all.temp_usage;
                set_result(all.build_result(std::move(results)));
            }
        });
}

void search_task_t::when_all(
    std::vector<search_task_t> tasks,
    std::shared_ptr<usage_t> temp_usage,
    std::function<expr_t(std::vector<expr_t>)> f)
{
    assert(not done());
    assert(std::ranges::all_of(tasks, [&] (auto const& t) { return t.usage == temp_usage; }));
    m_kind = all_t{std::move(tasks), std::move(temp_usage), std::move(f)};
}

void search_task_t::when_any(std::vector<search_task_t> tasks)
{
    assert(not done());
    m_kind = any_t{std::move(tasks)};
}

std::optional<expr_t>
search_proof(
    env_t const& env,
    ctx_t const& ctx,
    expr_t const& target,
    ast::is_mutable_t const is_mutable_allowed,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    auto st =
        search_state_t(
            env,
            ctx,
            target,
            is_mutable_allowed,
            std::make_shared<usage_t>(usage.extend()),
            usage_multiplier);
    do
        st.main_task.run();
    while (not st.main_task.done());
    std::optional<expr_t> result;
    if (st.main_task.succeeded())
    {
        usage += *st.main_task.usage;
        result.emplace(std::move(st.main_task.result()));
    }
    return result;
}

template <typename... T>
std::vector<search_task_t> make_sub_tasks(search_task_t& task, T&&... tactics)
{
    return {
        search_task_t(
            task.state,
            task.depth + 1,
            task.target,
            task.is_mutable_allowed,
            task.usage,
            task.usage_multiplier,
            tactics)...
    };
}

void proof_search(search_task_t& task)
{
    if (auto const it = task.state.cache.find(*task.target); it != task.state.cache.end())
        task.set_result(it->second);
    else
        task.when_any(make_sub_tasks(task, search_var, search_true_t, search_trivial_value, search_app));
}

} // namespace dep0::typecheck

