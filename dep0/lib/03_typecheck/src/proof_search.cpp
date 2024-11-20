#include "private/proof_search.hpp"

#include "private/tactics/search_app.hpp"
#include "private/tactics/search_trivial_value.hpp"
#include "private/tactics/search_true_t.hpp"
#include "private/tactics/search_var.hpp"

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/hash_code.hpp"

#include "dep0/match.hpp"
#include "dep0/tracing.hpp"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <unordered_map>

static std::atomic<std::uint64_t> next_task_id = 0ul;

namespace dep0::typecheck {

struct search_depth_friend_t { };
search_depth_t::search_depth_t(search_depth_friend_t const&, std::size_t const value) : m_value(value) { }

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

    std::shared_ptr<search_task_t> main_task;

    search_state_t(
        env_t const& env,
        ctx_t const& ctx,
        expr_t const& target,
        ast::is_mutable_t const is_mutable_allowed,
        std::shared_ptr<usage_t> usage,
        ast::qty_t const usage_multiplier)
    :   env(env),
        ctx(ctx),
        main_task(search_task_t::create(
            "main_task",
            std::weak_ptr<search_task_t>(),
            *this,
            search_depth_t(search_depth_friend_t{}, 0ul),
            std::make_shared<expr_t>(target),
            is_mutable_allowed,
            std::move(usage),
            usage_multiplier,
            proof_search))
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
    private_t,
    std::string name,
    std::weak_ptr<search_task_t> parent,
    search_state_t& st,
    search_depth_t depth,
    std::shared_ptr<expr_t const> target,
    ast::is_mutable_t const is_mutable_allowed,
    std::shared_ptr<usage_t> usage,
    ast::qty_t const usage_multiplier,
    std::function<void(search_task_t&)> task)
:   m_kind(one_t{std::move(task)}),
    m_target_str([&]
    {
        std::ostringstream os;
        if (TRACE_EVENT_CATEGORY_ENABLED("proof_search"))
            ast::pretty_print(os, *target);
        return os.str();
    }()),
    task_id(next_task_id.fetch_add(1ul, std::memory_order_relaxed)),
    name(std::move(name)),
    parent(std::move(parent)),
    state(st),
    depth(depth),
    env(st.env),
    ctx(st.ctx),
    target(std::move(target)),
    is_mutable_allowed(is_mutable_allowed),
    usage(std::move(usage)),
    usage_multiplier(usage_multiplier)
{ }

std::shared_ptr<search_task_t> search_task_t::create(
    std::string name,
    std::weak_ptr<search_task_t> parent,
    search_state_t& st,
    search_depth_t const depth,
    std::shared_ptr<expr_t const> target,
    ast::is_mutable_t const is_mutable_allowed,
    std::shared_ptr<usage_t> usage,
    ast::qty_t const usage_multiplier,
    std::function<void(search_task_t&)> task)
{
    return std::make_shared<search_task_t>(
        private_t{},
        std::move(name),
        std::move(parent),
        st,
        depth,
        std::move(target),
        is_mutable_allowed,
        std::move(usage),
        usage_multiplier,
        std::move(task)
    );
}

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
    if (depth.value() > 10ul or state.expired())
        return set_failed();
    TRACE_EVENT(TRACE_PROOF_SEARCH, perfetto::DynamicString(name), perfetto::Flow(task_id),
        "target", m_target_str, "depth", depth.value());
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
                if (not t->done())
                    t->run();
                if (t->succeeded())
                    return set_result(std::move(t->result()));
            }
            if (std::ranges::all_of(any.sub_tasks, [] (auto const& t) { return t->failed(); }))
                set_failed();
        },
        [this] (all_t& all)
        {
            for (auto& t: all.sub_tasks)
                if (not t->done())
                {
                    t->run();
                    if (t->failed())
                        return set_failed(); // if any sub-tasks fails, the parent task automatically fails
                }
            if (std::ranges::all_of(all.sub_tasks, [] (auto const& t) { return t->succeeded(); }))
            {
                std::vector<expr_t> results;
                results.reserve(all.sub_tasks.size());
                for (auto& t: all.sub_tasks)
                    results.push_back(std::move(t->result()));
                if (not usage->try_add(ctx, *all.temp_usage))
                    return set_failed();
                set_result(all.build_result(std::move(results)));
            }
        });
}

void search_task_t::when_all(
    std::vector<std::shared_ptr<search_task_t>> tasks,
    std::shared_ptr<usage_t> temp_usage,
    std::function<expr_t(std::vector<expr_t>)> f)
{
    assert(not done());
    assert(std::ranges::all_of(tasks, [&] (auto const& t) { return t->usage == temp_usage; }));
    m_kind = all_t{std::move(tasks), std::move(temp_usage), std::move(f)};
}

void search_task_t::when_any(std::vector<std::shared_ptr<search_task_t>> tasks)
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
    TRACE_EVENT(TRACE_PROOF_SEARCH, "search_proof()");
    do
        st.main_task->run();
    while (not st.main_task->done());
    std::optional<expr_t> result;
    if (st.main_task->succeeded())
        if (usage.try_add(ctx, *st.main_task->usage))
            result.emplace(std::move(st.main_task->result()));
    return result;
}

template <typename... T>
std::vector<std::shared_ptr<search_task_t>>
make_sub_tasks(search_task_t& parent, search_depth_t const new_depth, std::pair<char const*, T>... tactics)
{
    return {
        search_task_t::create(
            tactics.first,
            parent.weak_from_this(),
            parent.state,
            new_depth,
            parent.target,
            parent.is_mutable_allowed,
            parent.usage,
            parent.usage_multiplier,
            std::move(tactics.second))...
    };
}

/** Returns true if this task has the same target of any ancestor tasks. */
static bool detect_loop(search_task_t& task)
{
    auto p = task.parent;
    while (auto q = p.lock())
    {
        if (search_state_t::eq_t{}(*q->target, *task.target))
            return true;
        p = q->parent;
    }
    return false;
}

static void proof_search_impl(search_task_t& task, bool const run_search_app)
{
    if (auto const it = task.state.cache.find(*task.target); it != task.state.cache.end())
    {
        if (task.usage->try_add(task.ctx, it->second, task.usage_multiplier))
        {
            task.set_result(it->second);
            return;
        }
        // If we get here we found a cached value that cannot be used because it would use too many resources.
        // But we can still try to find another value which uses different resources, so we carry on.
    }
    if (detect_loop(task))
        task.set_failed();
    else
    {
        auto const new_depth = search_depth_t(search_depth_friend_t{}, task.depth.value() + 1ul);
        task.when_any(
            run_search_app
            ? make_sub_tasks(task, new_depth,
                std::pair{"search_var", search_var},
                std::pair{"search_true_t", search_true_t},
                std::pair{"search_trivial_value", search_trivial_value},
                std::pair{"search_app", search_app})
            : make_sub_tasks(task, new_depth,
                std::pair{"search_var", search_var},
                std::pair{"search_true_t", search_true_t},
                std::pair{"search_trivial_value", search_trivial_value})
        );
    }
}

void proof_search(search_task_t& task) { proof_search_impl(task, true); }
void quick_search(search_task_t& task) { proof_search_impl(task, false); }

} // namespace dep0::typecheck

