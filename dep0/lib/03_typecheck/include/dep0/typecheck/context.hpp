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
 * Unscoped contexts only allow typechecking of reference types if variables were bound to scoped contexts.
 * Scopes are given an internal ID, which increases when a new scope extends another.
 * This ID can be used to compare the lifetime of variables bound to a scope.
 * Specifically, smaller ID means larger scope.
 * However, if a variable is bound to an unscoped context, this ID is not provided,
 * thus indicating the unscoped origin of the variable.
 *
 * @see `dep0::typecheck::env_t`
 */
class ctx_t
{
public:

    /** @brief Tag type passed to the constructor in order to explicitly create a scoped context. */
    struct scoped_t{};

    /**
     * @brief Variable declaration stored in the context by their binidng variable.
     *
     * The user can only obtain a value of this type by doing a lookup,
     * thus proving that the user must have indeed done a lookup.
     */
    struct decl_t
    {
        friend class ctx_t;
        decl_t(
            std::optional<source_loc_t> origin,
            std::optional<std::size_t> scope_id,
            expr_t::var_t var,
            ast::qty_t const qty,
            expr_t type
        ) : origin(std::move(origin)),
            scope_id(std::move(scope_id)),
            var(std::move(var)),
            qty(qty),
            type(std::move(type))
        { }

    public:
        std::optional<source_loc_t> origin; /**< Where in the source code the variable declaration was encountered. */
        std::optional<std::size_t> scope_id; /**< Scope ID of the originating context or empty if unscoped. */
        expr_t::var_t var;  /**< Copy of the variable to which this declaration was bound, eg `x` in `0 i32_t x`. */
        ast::qty_t qty;     /**< Quantity of the variable declaration, eg `0` in `0 i32_t x`. */
        expr_t type;        /**< Type of the variable declaration, eg `i32_t` in `0 i32_t x`. */
    };

    /** @brief The default context is unscoped, which reduces the risk of compiler bugs when typechecking references. */
    ctx_t() = default;
    explicit ctx_t(scoped_t);
    ctx_t(ctx_t const&) = delete;               /**< @brief Copying is expensive. Consider using `extend()`. */
    ctx_t& operator=(ctx_t const&) = delete;    /**< @brief Copying is expensive. Consider using `extend()`. */
    ctx_t(ctx_t&&) = default;
    ctx_t& operator=(ctx_t&&) = default;

    // const member functions

    /** If the current context is scoped, return its ID; empty otherwise. */
    std::optional<std::size_t> scope() const;

    /**
     * @brief Obtain a fresh context that inherits from the current one, which is referred to as the "parent".
     *
     * The new context will allow to rebind variable names already bound in the parent context, aka shadowing.
     * The new context will be a scoped context if the parent was also scoped; otherwise will be unscoped.
     */
    ctx_t extend() const;

    /** @brief Like `extend()` but the new context will scoped even if the parent was unscoped. */
    ctx_t extend_scoped() const;

    /** @brief Like `extend()` but the new context will be unscoped even if the parent was scoped. */
    ctx_t extend_unscoped() const;

    /**
     * @brief Obtain a new context where all types have been rewritten according to the equality `from = to`.
     *
     * For example, if `xs` has type `array_t(i32_t, n)`, rewriting from `n` to `2` will
     * return a new context where `xs` has type `array_t(i32_t, 2)`.
     */
    ctx_t rewrite(expr_t const& from, expr_t const& to) const;

    /** @brief Return a complete snapshot of all declarations, including all parents and all shadowed variables. */
    std::vector<std::reference_wrapper<decl_t const>> decls() const;

    /**
     * @brief Return the expression bound to the given variable name, or nullptr if no binding exists yet.
     *
     * @remarks The returned pointer is guaranteed stable, i.e. it will never be invalidated.
     */
    decl_t const* operator[](expr_t::var_t const&) const;

    /**
     * @brief Return the expression bound to the innermost variable of the given name, or nullptr if none exists yet.
     *
     * @remarks The returned pointer is guaranteed stable, i.e. it will never be invalidated.
     */
    decl_t const* operator[](source_text const&) const;

    // non-const member functions

    /** @brief Add a new variable declaration with the given type and quantity and an automatically generated name. */
    void add_unnamed(ast::qty_t, expr_t type);

    /**
     * @brief Add a new binding variable to the current context level, if one does not already exist.
     *
     * If a binding already exists, but only at the parent level, the new binding will shadow the parent one.
     *
     * If the variable name is nullopt, an automatically generated name will be used as if `add_unnamed()` was invoked.
     */
    dep0::expected<expr_t::var_t> try_emplace(source_text, std::optional<source_loc_t>, ast::qty_t, expr_t type);

private:
    enum class scope_flavour_t { scoped_v, unscoped_v };
    scope_flavour_t m_flavour = scope_flavour_t::unscoped_v;
    std::size_t m_scope_id = 0ul;
    scope_map<source_text, expr_t::var_t> m_index;
    scope_map<expr_t::var_t, decl_t> m_values;

    ctx_t(
        scope_flavour_t,
        std::size_t new_scope_id,
        scope_map<source_text, expr_t::var_t>,
        scope_map<expr_t::var_t, decl_t>);
};

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
