#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/delta_reduction.hpp"

namespace dep0::ast {

template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, module_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, func_def_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, body_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, stmt_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, expr_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, type_t<P>&);

} // namespace dep0::ast

#include "dep0/ast/beta_delta_reduction_impl.hpp"
