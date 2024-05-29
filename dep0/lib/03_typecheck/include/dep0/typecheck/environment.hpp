#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/scope_map.hpp"

#include <set>
#include <variant>

namespace dep0::typecheck {

/**
 * An environment contains the definitions that can be unfolded during delta-reduction.
 */
class env_t
{
public:
    using value_type = std::variant<type_def_t, axiom_t, extern_decl_t, func_decl_t, func_def_t>;

    env_t() = default;
    env_t(env_t const&) = default;
    env_t& operator=(env_t const&) = default;
    env_t(env_t&&) = default;
    env_t& operator=(env_t&&) = default;

    // const member functions
    env_t extend() const;

    /**
     * Return the name of all globals visible from the current environment, i.e.
     * all globals in the current environment plus all globals from parent, grand-parent, etc.
     */
    std::set<expr_t::global_t> globals() const;

    value_type const* operator[](expr_t::global_t const&) const;

    // non-const member functions
    dep0::expected<std::true_type> try_emplace(expr_t::global_t, value_type);

private:
    // Function declarations are stored separately from their definitions (and from type definitions).
    // But the subscript operator must return an immutable stable pointer to a value_type,
    // so both map store a value_type bot not all alternatives are inhabited in either of them.
    scope_map<expr_t::global_t, value_type> m_fwd_decls;
    scope_map<expr_t::global_t, value_type> m_definitions;

    env_t(
        scope_map<expr_t::global_t, value_type> fwd_decls,
        scope_map<expr_t::global_t, value_type> definitions);
};

} // namespace dep0::typecheck
