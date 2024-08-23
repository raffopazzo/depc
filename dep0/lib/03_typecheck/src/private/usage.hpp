#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/scope_map.hpp"

#include <functional>

namespace dep0::typecheck {

/**
 * Helper object to keep track of how many times every variable has been used inside a function.
 * During type-checking/type-assignment this object is used to count the number of times
 * that a variable has been used and compare against the maximum allowed by the current context.
 * Typically the usage of a variable should be scaled up by a factor (aka "usage multiplier")
 * in order to take into account the compound effect of nested usages.
 * For example, using variable `x` on its own would count as `one` but,
 * if it is passed to a function argument with multiplicity `zero`, then overall it counts as `zero`;
 * conversely, if the function argument had multiplicity `many` then `x` should count as `many`.
 * Applying the correct "usage multiplier" is the responsiblity of the caller.
 * Moreover, usage objects can be nested inside other usage objects via the method `extend`.
 * This allows to keep track of "direct" usages, say from inside an if-else branch, whilst
 * still being able to lookup the total usage as-of before entering the branch.
 * Because of this, we talk about "direct" vs "inherited" usage count.
 */
class usage_t
{
    scope_map<expr_t::var_t, ast::qty_t> count;

    explicit usage_t(scope_map<expr_t::var_t, ast::qty_t>);

public:

    usage_t() = default;

    /**
     * Merge two usage objects into one using the given function to resolve conflicts.
     * @remarks Only direct usages are merged; inherited usages are ignored.
     */
    static usage_t merge(
        usage_t const&,
        usage_t const&,
        std::function<ast::qty_t(expr_t::var_t const&, ast::qty_t, ast::qty_t)>);

    /**
     * Obtain a new object to track usage of variables within a nested scope,
     * for example inside the true branch of an if-else statement.
     */
    usage_t extend() const;

    /**
     * Look up the current total usage of a variable;
     * returns `zero` if the variable has never been used so far.
     */
    ast::qty_t operator[](expr_t::var_t const&) const;

    /**
     * Add 1 usage of a variable, times the given multiplier, to its total count unless
     * the context in which it was declared does not allow for it.
     * For example, it is not allowed to use `x` in a context where its multiplicity is 0,
     * unless the usage multiplier is also 0.
     *
     * @remarks If usage is not allowed, the current usage count is left unchanged.
     *
     * @param context_lookup
     *      This method can only be invoked after proving that the variable is declared in its context.
     *
     * @param usage_multiplier
     *      The cost to add for the use of this variable is `1 * usage_multiplier`.
     *
     * @param loc
     *      If usage is not possible, copy this in the error message returned.
     */
    expected<std::true_type> try_add(
        context_lookup_t const& context_lookup,
        ast::qty_t usage_multiplier,
        std::optional<source_loc_t> loc = std::nullopt);

    /**
     * Add all usages of variables that appear inside the given expression, times the given multiplier, and
     * check that the result is allowed by the given context.
     * For example, it is not allowed to use the expression `x + x`
     * in a context where `x` has multiplicity 1,
     * unless the multiplier is 0.
     *
     * @remarks If usage is not allowed, the current usage count is left unchanged.
     */
    expected<std::true_type> try_add(ctx_t const&, expr_t const&, ast::qty_t usage_multiplier);

    /**
     * Add all uses from the given usage object to the current one and
     * check that the result is allowed by the given context.
     * Usually the other usage count is a direct extension of this one;
     * in fact, the parent object of the other usage object is not considered.
     *
     * @remarks If usage is not allowed, the current usage count is left unchanged.
     */
    expected<std::true_type> try_add(ctx_t const&, usage_t const&);
};

} // namespace dep0::typecheck
