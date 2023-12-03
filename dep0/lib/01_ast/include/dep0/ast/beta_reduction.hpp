#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P> bool beta_normalize(module_t<P>&);
template <Properties P> bool beta_normalize(func_def_t<P>&);
template <Properties P> bool beta_normalize(body_t<P>&);
template <Properties P> bool beta_normalize(stmt_t<P>&);
template <Properties P> bool beta_normalize(expr_t<P>&);

} // namespace dep0::ast

#include "dep0/ast/beta_reduction_impl.hpp"
