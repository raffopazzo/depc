/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::typecheck::is_mutable()` and associated overloads.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * @brief Return true if the given expression contains mutable operations.
 *
 * In other words, if multiple evaluations of this expression may result in different values.
 * For example invocations of mutable functions.
 */
bool is_mutable(expr_t const&);

/**
 * @brief Return true if the given application is mutable or contains any other mutable operations.
 *
 * In other words, if multiple evaluations of this expression may result in different values.
 * For example:
 *   - a call to a mutable function, either with or without arguments;
 *   - a call to an immutable function if any argument is the result of a mutable function call;
 *   - a call to an immutable function if any argument is the result of any other mutable operation.
 */
bool is_mutable(expr_t::app_t const&);

} // namespace dep0::typecheck
