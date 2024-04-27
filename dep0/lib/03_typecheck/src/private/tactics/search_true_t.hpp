#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "private/usage.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * Search for a value of type `true_t(expr)` for some boolean `expr`.
 * If `expr` is provably true, the returned value will be the empty initializer list `{}`,
 * because `true_t(true)` is a unit-like type.
 */
std::optional<expr_t>
search_true_t(
    environment_t const&,
    context_t const&,
    expr_t const&,
    usage_t&,
    ast::qty_t usage_multiplier);

} // namespace dep0::typecheck

