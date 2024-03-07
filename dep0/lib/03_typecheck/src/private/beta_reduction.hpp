#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

/**
 * @return true if beta normalization modified the given body.
 */
bool beta_normalize(body_t&);

/**
 * @return true if beta normalization modified the given expression.
 */
bool beta_normalize(expr_t&);

} // namespace dep0::typecheck
