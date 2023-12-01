#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"

namespace dep0::typecheck {

// Inspired by Idris and Coq proof states.
struct proof_state_t
{
    context_t context;
    sort_t goal;

    proof_state_t() = default;
    proof_state_t(context_t, sort_t);

    void rewrite(expr_t const& from, expr_t const& to);
};

} // namespace dep0::typecheck
