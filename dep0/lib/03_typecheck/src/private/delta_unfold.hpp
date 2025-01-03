/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Declares `dep0::typecheck::delta_unfold()` and associated overloads.
 * @see @ref delta_reduction
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

/**
 * @brief Unfold one function definition, from the given environment and context, inside the given body.
 * @return True if a definition was unfolded.
 * @see @ref delta_reduction
 */
bool delta_unfold(env_t const&, ctx_t const&, body_t&);

/**
 * @brief Unfold one function definition, from the given environment and context, inside the given expression.
 * @return True if a definition was unfolded.
 * @see @ref delta_reduction
 */
bool delta_unfold(env_t const&, ctx_t const&, expr_t&);

} // namespace dep0::typecheck
