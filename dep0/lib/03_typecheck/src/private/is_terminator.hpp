/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::typecheck::is_terminator()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * @brief Return true if the given statement will terminate the parent function.
 *
 * For example a `return` statement or an `if-else` which returns from both branches.
 *
 * @remarks
 *      An `if` statement without the else-branch is not considered a terminator;
 *      the function will eventually contain a terminator but this statement is not the one.
 *      By a similar argument, an `if-else` statement whose either branch does not
 *      contain a terminator is not considered a terminator either.
 */
bool is_terminator(stmt_t const&);

} // namespace dep0::typecheck
