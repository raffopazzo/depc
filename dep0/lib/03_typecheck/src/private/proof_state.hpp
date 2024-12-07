/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::typecheck::proof_state_t`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"

namespace dep0::typecheck {

/**
 * @brief Holds together the current context and return type (aka goal) of a body of execution statements.
 *
 * A proof state can be refined by using new knowledge that is discovered along the execution path.
 * For example, an `if (x)` statement without the `false` branch which returns from all paths of the `true` branch,
 * means that the condition `x` was false once execution gets past the `if` statement.
 * In this example, refining the proof statement means that a rewrite rule `x = false` can be
 * used to rewrite all types in both the context and the goal.
 *
 * @remarks The name "proof state" is inspired by Idris and Coq, but theirs is much more sophisticated.
 */
struct proof_state_t
{
    ctx_t context;
    sort_t goal; /**< Return type of the current body of execution. */

    proof_state_t(ctx_t, sort_t);

    /**
     * @brief Rewrite all types in both the context and the goal via the rewrite rule `from = to`.
     *
     * For example, the rewrite rule `x = false` transforms the type `true_t(x)` into `true_t(false)`.
     */
    void rewrite(expr_t const& from, expr_t const& to);
};

} // namespace dep0::typecheck
