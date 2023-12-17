#pragma once

#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/parser/ast.hpp"

namespace dep0::typecheck {

/**
 * Assign a type to the given expression in the given context.
 * 
 * Currently, not all expressions can be assigned a type.
 * In particular, numerical expressions and initializer lists,
 * as their type is either context-dependent or bound by a
 * target type.
 * 
 * @return The new expression with its type-assigned in the
 *         properties field.
 */
expected<expr_t> type_assign(context_t const&, parser::expr_t const&);

/**
 * Assign a type to an expression of the form `f(x...)`, for
 * some expression `f`, whether variable or sub-exression.
 * 
 * @return The new expression with its type-assigned in the
 *         properties field.
 */
expected<expr_t> type_assign_app(context_t const&, parser::expr_t::app_t const&, source_loc_t const&);



} // namespace dep0::typecheck
