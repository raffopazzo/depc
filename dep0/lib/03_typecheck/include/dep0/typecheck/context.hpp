#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/scope_map.hpp"

#include <ostream>
#include <set>

namespace dep0::typecheck {

/**
 * A context contains terms currently bound to binding variables, i.e. the function scope.
 */
class context_t
{
public:
    struct var_decl_t
    {
        ast::qty_t qty;
        expr_t type;
    };

    struct value_type
    {
        std::optional<source_loc_t> origin;
        var_decl_t value;
    };

    context_t() = default;
    context_t(context_t const&) = default;
    context_t& operator=(context_t const&) = default;
    context_t(context_t&&) = default;
    context_t& operator=(context_t&&) = default;

    // const member functions

    /**
     * Obtain a fresh context that inherits from the current one, which is referred to as the "parent".
     * The new context will allow to rebind variable names already bound in the parent context, aka shadowing.
     */
    context_t extend() const;

    /**
     * Obtain a new context where all types have been rewritten according to the equality `from = to`.
     * For example, if `xs` has type `array_t(i32_t, n)`, rewriting from `n` to `2` will
     * return a new context where `xs` has type `array_t(i32_t, 2)`.
     */
    context_t rewrite(expr_t const& from, expr_t const& to) const;

    /**
     * Return the name of all variables visible from the current context, i.e.
     * all variables in the current context plus all variables from parent, grand-parent, etc.
     */
    std::set<expr_t::var_t> vars() const;

    /**
     * Return the expression bound to the given variable name, or nullptr if no binding exists yet.
     * The returned pointer is guaranteed stable, i.e. it will never be invalidated.
     */
    value_type const* operator[](expr_t::var_t const&) const;

    // non-const member functions

    /**
     * Add a new binding variable to the current context level, with an automatically generated name.
     */
    void add_auto(var_decl_t);

    /**
     * Add a new binding variable to the current context level, if one does not already exist.
     * If a binding already exists, but only at the parent level, the new binding will shadow the parent one.
     * If the variable name is nullopt, an automatically generated name will be used as if `add_auto()` was invoked.
     */
    dep0::expected<std::true_type> try_emplace(std::optional<expr_t::var_t>, std::optional<source_loc_t>, var_decl_t);

private:
    scope_map<expr_t::var_t, value_type> m_values;

    context_t(scope_map<expr_t::var_t, value_type>);
};

// non-member functions
std::ostream& pretty_print(std::ostream&, context_t const&);

} // namespace dep0::typecheck
