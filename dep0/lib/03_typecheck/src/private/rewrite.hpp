/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::typecheck::rewrite()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * @brief Applies the rewrite rule `from = to` inside the given type.
 *
 * In other words, it replaces all occurrences of `from` with `to` inside `type`.
 * For example, the rewrite rule `x = false` transforms the type `true_t(x)` into `true_t(false)`.
 *
 * @return The new type if the rewrite rule changed anything; `nullopt` if the rewrite rule was ineffective.
 */
std::optional<sort_t> rewrite(expr_t const& from, expr_t const& to, sort_t const& type);

} // namespace dep0::typecheck
