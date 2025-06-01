/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::typecheck::derivation_rules` and associated factory functions.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/derivation.hpp"

namespace dep0::typecheck {

/**
 * @brief Private friend of `dep0::typecheck::derivation_t` that must be used to construct derivation proofs.
 *
 * By being a private type we try to guarantee that a legal AST node was constructed by the typechecking stage.
 *
 * @remarks At the moment there is no requirements to make any derivation; but could add some, if useful.
 */
struct derivation_rules
{
    template <typename T>
    static derivation_t<T> make_derivation();

    /** @brief Constructs the term `typename` whose type is Kind. */
    static expr_t make_typename();

    /** @brief Constructs the term `true_t` whose type is `(0 bool_t) -> typename`. */
    static expr_t make_true_t();

    /**
     * @brief Constructs a `true_t(expr)` whose type is `typename`.
     *
     * @warning It is the caller's responsibility to ensure that the given expression has type `bool_t`.
     */
    static expr_t make_true_t(expr_t);

    /** @brief Constructs the expression `&x` whose type is `ref_t(element_type, scopeof(x))`. */
    static expr_t make_addressof(expr_t element_type, expr_t::scopeof_t);

    /** @brief Constructs the term `ref_t` whose type is `(typename, scope_t) -> typename`. */
    static expr_t make_ref_t();

    /** @brief Constructs the type `scope_t` whose type is `typename`. */
    static expr_t make_scope_t();

    /** @brief Constructs the type `bool_t` whose type is `typename`. */
    static expr_t make_bool();

    /** @brief Constructs the type `cstr_t` whose type is `typename`. */
    static expr_t make_cstr();

    /** @brief Constructs the type `unit_t` whose type is `typename`. */
    static expr_t make_unit();

    /** @brief Constructs the type `i8_t` whose type is `typename`. */
    static expr_t make_i8();

    /** @brief Constructs the type `i16_t` whose type is `typename`. */
    static expr_t make_i16();

    /** @brief Constructs the type `i32_t` whose type is `typename`. */
    static expr_t make_i32();

    /** @brief Constructs the type `i64_t` whose type is `typename`. */
    static expr_t make_i64();

    /** @brief Constructs the type `u8_t` whose type is `typename`. */
    static expr_t make_u8();

    /** @brief Constructs the type `u16_t` whose type is `typename`. */
    static expr_t make_u16();

    /** @brief Constructs the type `u32_t` whose type is `typename`. */
    static expr_t make_u32();

    /** @brief Constructs the type `u64_t` whose type is `typename`. */
    static expr_t make_u64();

    /** @brief Constructs a numeric constant of type `u64_t` containing the given value. */
    static expr_t make_u64(std::size_t);

    /** @brief Constructs the boolean value `true`. */
    static expr_t make_true();

    /** @brief Constructs the boolean value `false`. */
    static expr_t make_false();

    /** @brief Constructs an `expr_t::boolean_expr_t` containing the given value; it's type is `bool_t`. */
    static expr_t make_boolean_expr(expr_t::boolean_expr_t::value_t);

    /** @brief Constructs an `expr_t::relation_expr_t` containing the given value; it's type is `bool_t`. */
    static expr_t make_relation_expr(expr_t::relation_expr_t::value_t);

    /** @brief Constructs the term `array_t` whose type is `(0 typename, 0 u64_t) -> typename`. */
    static expr_t make_array();

    /**
     * @brief Constructs an `expr_t::app_t` from a function and its argument.
     *
     * The type of the returned expression is the return type of the function.
     *
     * @warning The type of the returned expression might depend on the arguments to which the function is applied.
     * It is the caller's responsibility to ensure that substitution of all arguments has been done correctly.
     */
    static expr_t make_app(expr_t func, std::vector<expr_t> args);

    /**
     * @brief Constructs an expression `obj[idx]`, of the given element type, for a constant index `idx`.
     *
     * @warning For dependent tuples, the type of `obj[idx]` may depend on the values of `obj[idx-1], obj[idx-2], ...`.
     * It is the caller's responsibility to ensure that substitution of all values has been done correctly.
     */
    static expr_t make_subscript(expr_t obj, std::size_t idx, sort_t type);
};

template <typename... Args>
module_t make_legal_module(Args&&... args)
{
    return module_t{derivation_rules::make_derivation<module_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
type_def_t make_legal_type_def(source_loc_t const origin, Args&&... args)
{
    return type_def_t{origin, derivation_rules::make_derivation<type_def_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
axiom_t make_legal_axiom(source_loc_t const origin, sort_t sort, Args&&... args)
{
    return axiom_t{
        origin,
        derivation_rules::make_derivation<axiom_t>(),
        std::move(sort),
        std::forward<Args>(args)...
    };
}

template <typename... Args>
extern_decl_t make_legal_extern_decl(source_loc_t const origin, sort_t sort, Args&&... args)
{
    return extern_decl_t{
        origin,
        derivation_rules::make_derivation<extern_decl_t>(),
        std::move(sort),
        std::forward<Args>(args)...
    };
}

template <typename... Args>
func_decl_t make_legal_func_decl(source_loc_t const origin, sort_t sort, Args&&... args)
{
    return func_decl_t{
        origin,
        derivation_rules::make_derivation<func_decl_t>(),
        std::move(sort),
        std::forward<Args>(args)...
    };
}

template <typename... Args>
func_def_t make_legal_func_def(source_loc_t const origin, sort_t sort, Args&&... args)
{
    return func_def_t{
        origin,
        derivation_rules::make_derivation<func_def_t>(),
        std::move(sort),
        std::forward<Args>(args)...
    };
}

template <typename... Args>
func_arg_t make_legal_func_arg(Args&&... args)
{
    return func_arg_t{derivation_rules::make_derivation<func_arg_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
body_t make_legal_body(Args&&... args)
{
    return body_t{derivation_rules::make_derivation<body_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
stmt_t make_legal_stmt(Args&&... args)
{
    return stmt_t{derivation_rules::make_derivation<stmt_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
expr_t make_legal_expr(sort_t sort, Args&&... args)
{
    return expr_t{derivation_rules::make_derivation<expr_t>(), std::move(sort), std::forward<Args>(args)...};
}

template <typename T>
derivation_t<T> derivation_rules::make_derivation()
{
    return derivation_t<T>{};
}

} // namespace dep0::typecheck
