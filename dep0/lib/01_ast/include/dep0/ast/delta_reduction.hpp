/**
 * @file delta_reduction.hpp
 * @brief A family of functions that perform a "simplified" one-step delta-reduction, not a complete normalization.
 *
 * By "simplified" we mean that one-step delta-reduction is not applied everywhere that is theoretically possible,
 * but only to function applications whose function expression is a variable name, eg `f(x, y, z)`.
 * In contrast, a "complete" one-step delta-reduction would normally expand `f` anywhere,
 * even if it appears on its own, for example inside `return f`.
 * The reason behind this decision is that we are interested in applying delta-reduction
 * only as a way to extend beta-normalization as far as possible.
 * Therefore there is no real value in expanding `f` when it appears on its own,
 * because this expansion will never be followed by a beta-reduction.
 * In fact, it may even have negative value, because during LLVM codegen we might end up
 * generating an anonymous private function when we could just refer to `f` by name.
 */

#pragma once

#include "dep0/ast/ast.hpp"

#include "dep0/scope_map.hpp"

namespace dep0::ast {

namespace delta_reduction {
    template <Properties P>
    using context_t =
        scope_map<
            typename expr_t<P>::var_t,
            std::variant<
                typename expr_t<P>::abs_t,
                struct something_else_t>>;
    struct something_else_t{};
}

/**
 * Performs a "simplified" one-step delta-reduction inside a function definition, @see delta_reduction.hpp.
 *
 * @return True if one step of delta-reduction was preformed, false otherwise.
 */
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, func_def_t<P>&);

/**
 * Performs a "simplified" one-step delta-reduction inside a body, @see delta_reduction.hpp.
 *
 * @return True if one step of delta-reduction was preformed, false otherwise.
 */
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, body_t<P>&);

/**
 * Performs a "simplified" one-step delta-reduction inside a statement, @see delta_reduction.hpp.
 *
 * @return True if one step of delta-reduction was preformed, false otherwise.
 */
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, stmt_t<P>&);

/**
 * Performs a "simplified" one-step delta-reduction inside an expression, @see delta_reduction.hpp.
 *
 * @return True if one step of delta-reduction was preformed, false otherwise.
 */
template <Properties P> bool delta_reduce(delta_reduction::context_t<P> const&, expr_t<P>&);

} // namespace dep0::ast

#include "dep0/ast/delta_reduction_impl.hpp"
