#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"

namespace dep0::typecheck {

/**
 * Holds together information about the current context and return type (aka goal) of a body of execution statements.
 * A proof state can be refined by using new knowledge that is discovered along the execution path.
 * For example, an `if (x)` statement without the `false` branch which returns from all paths of the `true` branch,
 * means that the condition `x` was false once execution gets past the `if` statement.
 * In this example, refining the proof statement means that a rewrite rule `x = false` can be
 * used to rewrite all types in both the context and the goal.
 *
 * The name "proof state" is inspired by Idris and Coq.
 */
struct proof_state_t
{
    context_t context;
    sort_t goal;

    proof_state_t() = default;
    proof_state_t(context_t, sort_t);

    void rewrite(expr_t const& from, expr_t const& to);
};

} // namespace dep0::typecheck
