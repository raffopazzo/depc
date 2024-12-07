/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-header function declaring the tactic `dep0::typecheck::search_trivial_value()`.
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
 * @brief If the target type is inhabited by a single value, this task succeeds and its result is that value.
 *
 * For example, user-defined integral whose only value is 0 or `array_t(t, 0)` whose only value is `{}`.
 *
 * @remarks This tactic will either succeed or fail immediately and is therefore used by `quick_search()`.
 */
void search_trivial_value(search_task_t&);

} // namespace dep0::typecheck
