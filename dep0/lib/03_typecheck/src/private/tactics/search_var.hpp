/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-header function declaring the tactic `dep0::typecheck::search_var()`.
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
 * @brief Search for a value of the target type inside the environemnt or context of the given task.
 *
 * If the environemnt contains a declaration or definition of the target type,
 * the task succeeds and its result is a `global_t` referring to it.
 * If the context contains a declaration of the target type,
 * the task succeeds and its result is a `var_t` referring to it.
 * Otherwise this task fails.
 *
 * @remarks This tactic will either succeed or fail immediately and is therefore used by `quick_search()`.
 */
void search_var(search_task_t&);

} // namespace dep0::typecheck
