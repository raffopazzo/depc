/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Return true if all branches of execution inside the given body contain impossible statements.
 * If all branches are impossible, the entire body can be removed from the final programme.
 */
bool is_impossible(body_t const&);

/**
 * Return true if all branches of execution inside the given range contain impossible statements.
 * If all branches are impossible, this entire portion of code can be removed from the final programme.
 */
bool is_impossible(std::vector<stmt_t>::const_iterator begin, std::vector<stmt_t>::const_iterator end);

} // namespace dep0::typecheck
