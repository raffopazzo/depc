/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Helper functions to test whether a type is of the 1st order.
 * @see @ref first_order_types
 * @page first_order_types First and Second Order Types
 * In @ref polymorphic_functions we shown 1st and 2nd order abstraction and application rules:
 *   - an abstraction of the 1st order is a function that takes a "regular" argument
 *   - an abstraction of the 2nd order is a function that takes a type
 *   - an application is of the 1st or 2nd order if the function applied is of the 1st or 2nd order, respectively.
 *
 * You can imagine extending this concept recursively to types:
 *   - primitive types are 1st order, eg `bool_t`, `i8_t`, etc
 *   - type variables are 2nd order, i.e. `typename t`
 *   - arrays of 1st order types are 1st order
 *   - function types taking only 1st order argument types (including 1st order functions) are also 1st order,
 *     for example `(i32_t x) -> bool_t` or `(i32_t x, (i32_t) -> bool_t f) -> bool_t`
 *   - Pi-types are therefore 2nd order, for example `(typename t, t) -> t`
 *   - dependent types are 2nd order, for example `true_t(x)`;
 *     note however that `true_t(true)` is 1st order instead.
 *
 * The concept of 1st and 2nd order types is particularly useful during the LLVM IR codegen stage because
 * 1st order types can be expressed by LLVM IR but 2nd order types cannot.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include <vector>

namespace dep0::llvmgen {

/**
 * @brief Checks whether a function type formed by the given arguments and return type is of the 1st order.
 * @see @ref first_order_types
 * @param args The arguments forming the function types; only their types matter for this purpose.
 * @param ret_type The return type of the function type.
 * @return True if the function type formed by the given arguments and return type is of the 1st order.
 */
bool is_first_order_function_type(std::vector<typecheck::func_arg_t> const&, typecheck::expr_t const& ret_type);

/**
 * @brief Checks whether a type is a 1st order type.
 * @see @ref first_order_types
 * @return True if the given type expression is a 1st order type; false if it has higher order.
 */
bool is_first_order_type(typecheck::expr_t const&);

} // namespace dep0::llvmgen
