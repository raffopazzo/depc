/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-header function declaring the tactic `dep0::typecheck::search_true_t()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "private/proof_search.hpp"
#include "private/usage.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * @brief If the target type has the form `true_t(expr)` and `expr` can be proved true,
 * this task succeeds and its result is the empty initializer list `{}`.
 *
 * If `expr` is not "obviously" true, this task fails.
 * By "obviously" we mean that no further search must be necessary to establish that it's true;
 * only compuations are allowed.
 * 
 * @remarks This tactic will either succeed or fail immediately and is therefore used by `quick_search()`.
 */
void search_true_t(search_task_t&);

} // namespace dep0::typecheck

