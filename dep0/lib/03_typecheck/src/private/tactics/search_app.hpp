/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-header function declaring the tactic `dep0::typecheck::search_app()`.
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
 * @brief Search for a value of the target type by applying axioms and theorems
 * (aka functions) from the given environment.
 *
 * @remarks This tactic may not complete immediately and is therefore **not** used by `quick_search()`.
 */
void search_app(search_task_t&);

} // namespace dep0::typecheck
