/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::typecheck::ctx_t`
 * @see @ref context
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/scope_map.hpp"

#include <optional>
#include <ostream>
#include <set>

namespace dep0::typecheck {

/**
 * @brief Contains local variable declarations, with their quantity and type.
 *
 * Contexts can be extended by inner contexts thus allowing shadowing of variable names.
 *
 * Also contexts come in two flavours: scoped and unscoped.
 * Scoped contexts represent a region of memory from which addresses can be taken.
 * Unscoped contexts only allow typechecking of reference types bound to parent scopes.
 * When extending a context, the inner context can only be scoped if the parent was also scoped.
 *
 * @see `dep0::typecheck::env_t`
 */
class ctx_t
{
public:

    /** @brief Tag type passed to the constructor in order to explicitly create a scoped context. */
    struct scoped_t{};

    /**
     * @brief Variable declarations need a type and a quantity, for example `0 typename t`.
     *
     * @remarks The variable name, `t` in this example, is instead stored as a key in the context.
     */
    struct var_decl_t
    {
        ast::qty_t qty;
        expr_t type;
    };

    /** @brief Value stored in the context, whose key is the variable name. */
    struct value_type
    {
        std::optional<source_loc_t> origin; /**< Where in the source code the variable declaration was encountered. */
        std::optional<std::size_t> scope_id; /**< Scope ID of the originating context or empty if unscoped. */
        var_decl_t value;
    };

    /** @brief The default context is unscoped, which reduces the risk of compiler bugs when typechecking references. */
    ctx_t() = default;
    explicit ctx_t(scoped_t);
    ctx_t(ctx_t const&) = default;
    ctx_t& operator=(ctx_t const&) = default;
    ctx_t(ctx_t&&) = default;
    ctx_t& operator=(ctx_t&&) = default;

    // const member functions

    std::optional<std::size_t> scope() const { return m_scope_id; }

    /**
     * @brief Obtain a fresh context that inherits from the current one, which is referred to as the "parent".
     *
     * The new context will allow to rebind variable names already bound in the parent context, aka shadowing.
     * The new context will be a scoped context if the parent was also scoped; otherwise will be unscoped.
     */
    ctx_t extend() const;

    /** @brief Like `extend()` but the new context will be unscoped even if the parent was scoped. */
    ctx_t extend_unscoped() const;

    /**
     * @brief Obtain a new context where all types have been rewritten according to the equality `from = to`.
     *
     * For example, if `xs` has type `array_t(i32_t, n)`, rewriting from `n` to `2` will
     * return a new context where `xs` has type `array_t(i32_t, 2)`.
     */
    ctx_t rewrite(expr_t const& from, expr_t const& to) const;

    /**
     * @brief Return the name of all variables visible from the current context.
     *
     * That is, all variables in the current context plus all variables from parent, grand-parent, etc.
     */
    std::set<expr_t::var_t> vars() const;

    /**
     * @brief Return the expression bound to the given variable name, or nullptr if no binding exists yet.
     *
     * @remarks The returned pointer is guaranteed stable, i.e. it will never be invalidated.
     */
    value_type const* operator[](expr_t::var_t const&) const;

    // non-const member functions

    /** @brief Add a new variable to the current context, with the given type and an automatically generated name. */
    void add_unnamed(expr_t);

    /**
     * @brief Add a new binding variable to the current context level, if one does not already exist.
     *
     * If a binding already exists, but only at the parent level, the new binding will shadow the parent one.
     *
     * If the variable name is nullopt, an automatically generated name will be used as if `add_unnamed()` was invoked.
     */
    dep0::expected<std::true_type> try_emplace(std::optional<expr_t::var_t>, std::optional<source_loc_t>, var_decl_t);

private:
    std::optional<std::size_t> m_scope_id;
    scope_map<expr_t::var_t, value_type> m_values;

    void add_unnamed(var_decl_t);

    ctx_t(scope_map<expr_t::var_t, value_type>, std::optional<std::size_t> new_scope_id);
};

/** @brief Proof that a variable exists inside some context, returned from `context_lookup`. */
class context_lookup_t
{
    context_lookup_t(ctx_t const&, expr_t::var_t, ctx_t::value_type const&);

public:
    ctx_t const& ctx; /**< @brief The context in which the variable was declared. */
    expr_t::var_t const var; /**< @brief The variable that was declared. */
    ctx_t::var_decl_t const& decl; /**< @brief The declaration bound to the given variable. */
    std::optional<std::size_t> const scope_id; /**< @brief Scope ID of the originating context or empty if unscoped. */

    /**
     * @brief Checks whether the given variable is declared inside the given context and
     * returns a proof of this fact or an empty optional otherwise.
     *
     * @warning
     *      The returned proof stores a reference to the context and the declaration,
     *      so passing a temporary context will lead to undefined behaviour.
     */
    friend std::optional<context_lookup_t> context_lookup(ctx_t const&, expr_t::var_t const&);
};
std::optional<context_lookup_t> context_lookup(ctx_t const&, expr_t::var_t const&);

// non-member functions
/**
 * @brief Prints the given context to the output stream with pretty formatting to make it human-readable.
 *
 * All declarations are printed in alphabetical order with
 * some effort to respect topological ordering of dependencies.
 *
 * Care is also taken to align variable names and their types in a table-like fashion, for example:
 *
 * ~~~plain
 *   n      : u64_t
 * 0 t      : typename
 * 1 xs     : array_t(t, n)
 * ~~~
 */
std::ostream& pretty_print(std::ostream&, ctx_t const&);

} // namespace dep0::typecheck
