#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::transform {

bool beta_normalize(typecheck::module_t&);
bool beta_normalize(typecheck::func_def_t&);
bool beta_normalize(typecheck::body_t&);
bool beta_normalize(typecheck::stmt_t&);
bool beta_normalize(typecheck::expr_t&);
bool beta_normalize(typecheck::type_t&);

} // namespace dep0::transform
