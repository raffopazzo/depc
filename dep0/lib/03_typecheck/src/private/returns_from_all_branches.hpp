/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Helper functions to test whether a body contains `return` statements in all possible branches.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * @brief Return true if all branches of the given body explicitly contain a `return` statement, false otherwise.
 *
 * For example:
 *
 *   - false for an empty body `{}`, because it does not contain a `return` statement at all;
 *   - true for an if-else statement that explicitly contains a `return` statement in both branches;
 *   - false for an `if` statement without the else-branch.
 */
bool returns_from_all_branches(body_t const&);

/** Return true if the input if-else statement contains both branches and both contain a return statement. */
bool returns_from_all_branches(stmt_t::if_else_t const&);

} // namespace dep0::typecheck
