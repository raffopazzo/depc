/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::typecheck::env_t`.
 * @see @ref delta_reduction
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/scope_map.hpp"

#include <set>
#include <variant>

namespace dep0::typecheck {

/**
 * @brief Global symbols are stored in an environment.
 *
 * For example:
 *   - type definitions
 *   - axioms
 *   - extern declarations
 *   - function declarations and their definitions
 *   - etc.
 *
 * These symbols can be introduced by:
 *   - the current moduble being compiled
 *   - or any other imported module.
 *
 * @see `dep0::typecheck::ctx_t`
 */
class env_t
{
public:

    /** @brief Placeholder marking a global symbol as an incomplete type, used for typechecking recursive structs. */
    struct incomplete_type_t { source_loc_t origin; };

    /** @brief Value stored in the environment, whose key is the global symbol name. */
    using value_type = std::variant<incomplete_type_t, type_def_t, axiom_t, extern_decl_t, func_decl_t, func_def_t>;

    /** @brief Empty environment containing no definitions, not even from the prelude module. */
    env_t() = default;

    env_t(env_t const&) = delete;               /**< @brief Copying is expensive. Consider using `extend()`. */
    env_t& operator=(env_t const&) = delete;    /**< @brief Copying is expensive. Consider using `extend()`. */
    env_t(env_t&&) = default;
    env_t& operator=(env_t&&) = default;

    // const member functions

    /**
     * @brief Return a new environment inheriting from the current one, allowing new symbols to shadow existing ones.
     *
     * The new environment will "see" all symbols from the parent environment,
     * but the parent environment will not see new symbols added to the new environment.
     */
    env_t extend() const;

    /**
     * @brief Return the name of all globals visible from the current environment.
     *
     * That is, all globals in the current environment plus all globals from parent, grand-parent, etc.
     */
    std::set<expr_t::global_t> globals() const;

    /**
     * @brief Return the entry referred to by the given global symbol or `nullptr` if none is found.
     * @remarks The returned pointer is guaranteed stable, i.e. it is not invalidated when new entries are added.
     */
    value_type const* operator[](expr_t::global_t const&) const;

    // non-const member functions
    /**
     * @brief Import all exported symbols from the given module into the current environment.
     *
     * For each exported symbol `sym`, a new entry with the qualified identifier `module_name::sym`
     * will be added to the current environment.
     *
     * @warning The prelude module must be imported with an empty module name `""`.
     */
    dep0::expected<std::true_type> import(source_text module_name, module_t const&);

    /** @brief Add a new entry to the current module, returning an error if the insertion fails. */
    dep0::expected<std::true_type> try_emplace(expr_t::global_t, value_type);

private:
    scope_map<expr_t::global_t, value_type> m_definitions;

    env_t(scope_map<expr_t::global_t, value_type> definitions);
};

/**
 * @brief Build a new environment with the prelude module already pre-imported,
 * suitable as a base to compile user modules.
 */
dep0::expected<env_t> make_base_env();

} // namespace dep0::typecheck
