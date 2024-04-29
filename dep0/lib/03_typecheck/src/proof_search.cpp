#include "private/proof_search.hpp"

#include "private/tactics/search_app.hpp"
#include "private/tactics/search_trivial_value.hpp"
#include "private/tactics/search_true_t.hpp"
#include "private/tactics/search_var.hpp"

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/hash_code.hpp"

#include "dep0/match.hpp"

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
};

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
    environment_t const& env,
    context_t const& ctx,
    expr_t const& type,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    search_state_t search_state;
    return continue_proof_search(env, ctx, type, search_state, usage, usage_multiplier);

}

std::optional<expr_t>
continue_proof_search(
    environment_t const& env,
    context_t const& ctx,
    expr_t const& type,
    search_state_t& st,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
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
    return result;
}

} // namespace dep0::typecheck

