/**
 * @file beta_delta_reduction.hpp
 * @brief A family of functions that perform a combined beta-delta normalization.
 *
 * The normalization strategy is to first apply a full beta-normalization,
 * which should help in reducing the size of the AST.
 * If no more beta-normalizations are possible, try to perform a single one-step delta-reduction,
 * (arguably it should be called a delta-expansion) which might unblock further beta-normalizations.
 * If delta-reduction was unsuccessful, then we have reached a beta-delta normal-form.
 */

#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/delta_reduction.hpp"

namespace dep0::ast {

template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, module_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, func_def_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, body_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, stmt_t<P>&);
template <Properties P> bool beta_delta_normalize(delta_reduction::context_t<P> const&, expr_t<P>&);

} // namespace dep0::ast

#include "dep0/ast/beta_delta_reduction_impl.hpp"
