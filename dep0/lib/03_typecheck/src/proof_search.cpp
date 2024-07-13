#include "private/proof_search.hpp"

#include "private/tactics/search_app.hpp"
#include "private/tactics/search_trivial_value.hpp"
#include "private/tactics/search_true_t.hpp"
#include "private/tactics/search_var.hpp"

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/hash_code.hpp"

#include "dep0/match.hpp"

#include <chrono>
#include <unordered_map>

namespace dep0::typecheck {

struct search_state_t
{
    struct eq_t { bool operator()(expr_t const&, expr_t const&) const; };
    struct hash_t { std::size_t operator()(expr_t const&) const; };

    struct in_progress_t{};
    using result_t = expr_t;
    using value_t = std::variant<in_progress_t, result_t>;
    using cache_t = std::unordered_map<expr_t, value_t, hash_t, eq_t>;
    cache_t cache;
    ast::is_mutable_t is_mutable_allowed;
    std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    std::size_t depth = 0;

    explicit search_state_t(ast::is_mutable_t const m) : is_mutable_allowed(m) {}
};

bool is_mutable_allowed(search_state_t const& s)
{
    return s.is_mutable_allowed == ast::is_mutable_t::yes;
}

bool search_state_t::eq_t::operator()(expr_t const& x, expr_t const& y) const
{
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

std::optional<expr_t>
start_proof_search(
    env_t const& env,
    ctx_t const& ctx,
    expr_t const& type,
    ast::is_mutable_t const is_mutable_allowed,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    search_state_t search_state(is_mutable_allowed);
    return continue_proof_search(env, ctx, type, search_state, usage, usage_multiplier);

}

std::optional<expr_t>
continue_proof_search(
    env_t const& env,
    ctx_t const& ctx,
    expr_t const& type,
    search_state_t& st,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    if (st.depth >= 10 or std::chrono::steady_clock::now() > st.deadline)
        return std::nullopt;
    ++st.depth;
    std::optional<expr_t> result;
    auto const [it, inserted] = st.cache.try_emplace(type, search_state_t::in_progress_t{});
    if (inserted)
    {
        // Try a bunch of tactics until the first one succeeds.
        auto const try_one = [&] (auto&& f) { if (not result) result = f(env, ctx, type, st, usage, usage_multiplier); };
        auto const try_all = [&] (auto&&... tactics) { (try_one(tactics), ...); };
        try_all(
            search_var,
            search_true_t,
            search_trivial_value,
            search_app
        );
        if (result)
            it->second = *result;
    }
    // If a search for this same type was already in progress, then we fail; clearly this is going nowhere.
    // Otherwise a previous search succeeded and we can return the previous result.
    else if (auto const prev_result = std::get_if<expr_t>(&it->second))
        result.emplace(*prev_result);
    --st.depth;
    return result;
}

} // namespace dep0::typecheck

