#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/scope_map.hpp"

#include <set>
#include <variant>

namespace dep0::typecheck {

/**
 * An environment contains global symbols such as:
 *   - type definitions
 *   - axioms
 *   - extern declarations
 *   - function declarations and their definitions
 *   - etc.
 * These symbols can be introduced by:
 *   - the current moduble being compiled
 *   - or any other imported module.
 */
class env_t
{
public:

    /** The type of entries that can be stored in an environment. */
    using value_type = std::variant<type_def_t, axiom_t, extern_decl_t, func_decl_t, func_def_t>;

    /** Build an empty environment containing no definitions, not even from the prelude module. */
    env_t() = default;

    env_t(env_t const&) = default;
    env_t& operator=(env_t const&) = default;
    env_t(env_t&&) = default;
    env_t& operator=(env_t&&) = default;

    // const member functions

    /**
     * Return a new environment inheriting from the current one, allowing new symbols to shadow existing ones.
     * The new environment will "see" all symbols from the parent environment,
     * but the parent environment will not see new symbols added to the new environment.
     */
    env_t extend() const;

    /**
     * Return the name of all globals visible from the current environment, i.e.
     * all globals in the current environment plus all globals from parent, grand-parent, etc.
     */
    std::set<expr_t::global_t> globals() const;

    /**
     * Return the entry referred to by the given global symbol or `nullptr` if none is found.
     * @remarks The returned pointer is guaranteed stable, i.e. it is not invalidated when new entries are added.
     */
    value_type const* operator[](expr_t::global_t const&) const;

    // non-const member functions
    /**
     * Import all exported symbols from the given module into the current environment.
     * For each exported symbol `sym`, a new entry with the qualified identifier `module_name::sym`
     * will be added to the current environment.
     * The prelude module should be imported with an empty module name `""`.
     */
    dep0::expected<std::true_type> import(source_text module_name, module_t const&);

    /** Add a new entry to the current module, returning an error if the insertion fails. */
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

/**
 * Build a new environment with the prelude module already pre-imported,
 * suitable as a base to compile user modules.
 */
dep0::expected<env_t> make_base_env();

} // namespace dep0::typecheck
