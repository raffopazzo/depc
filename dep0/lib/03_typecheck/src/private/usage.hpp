#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/scope_map.hpp"

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
 */
class usage_t
{
    scope_map<expr_t::var_t, ast::qty_t> count;

    explicit usage_t(scope_map<expr_t::var_t, ast::qty_t>);

public:

    usage_t() = default;

    /**
     * Obtain a new object to track usage of variables within a nested scope,
     * for example inside the true branch of an if-else statement.
     */
    usage_t extend() const;

    /**
     * Add usage of a variable to its total count and return the new total.
     */
    ast::qty_t add(expr_t::var_t const&, ast::qty_t);

    /**
     * Look up the current total usage of a variable;
     * returns `zero` if the variable has never been used so far.
     */
    ast::qty_t operator[](expr_t::var_t const&) const;

    /**
     * Add uses from another context to this one.
     * Usually the other context is a direct extension of this one;
     * in fact, the parent context of the input object is not considered.
     * This is useful, for example, to tally up usages of both branches of an if-else statement,
     * or to add usages from a temporary usage object, for example during proof-search.
     */
    usage_t& operator+=(usage_t const&);
};

} // namespace dep0::typecheck
