#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

bool beta_normalize(module_t&);
bool beta_normalize(body_t&);
bool beta_normalize(expr_t&);

} // namespace dep0::typecheck
