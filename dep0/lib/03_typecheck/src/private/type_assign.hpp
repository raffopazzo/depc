#pragma once

#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/parser/ast.hpp"

namespace dep0::typecheck {

/**
 * Assign a type to the given expression in the given context.
 * 
 * Currently, not all expressions can be assigned a type.
 * In particular, numerical expressions and initializer lists, as their type is
 * either context-dependent or bound by a target type.
 * 
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t> type_assign(context_t const&, parser::expr_t const&);

/**
 * Assign a type to a function application.
 * 
 * @param loc The location in the source file where the expression was found.
 *            If type-assignment fails, it will be copied in the error message.
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t> type_assign_app(context_t const&, parser::expr_t::app_t const&, source_loc_t const& loc);

/**
 * Assign a type to an abstraction.
 *
 * @param loc   The location in the source file where the expression was found.
 *              If type-assignment fails, it will be copied in the error message.
 * @param name  If the abstraction is given a name, it can call itself
 *              recursively.
 *
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t>
type_assign_abs(
    context_t const&,
    parser::expr_t::abs_t const&,
    source_loc_t const& loc,
    std::optional<source_text> const& name);

} // namespace dep0::typecheck
