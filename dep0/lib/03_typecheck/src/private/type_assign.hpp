#pragma once

#include "private/usage.hpp"

#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/parser/ast.hpp"

namespace dep0::typecheck {

/**
 * Assign a type to the given expression in the given environment and context.
 * 
 * Not all expressions can be assigned a type when taken in isolation;
 * for example numerical expressions and initializer lists.
 *
 * @param usage Helper object to keep track of usage of variables in the given context.
 *
 * @param usage_multiplier  When adding up usage of variables, their quantity is multiplied by this amount
 *                          in order to account for nested usage. For example, in an erased context this
 *                          multiplier can be `zero` or when checking against an unrestricted function
 *                          argument the multiplier should be `many`.
 * 
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t>
type_assign(
    environment_t const&,
    context_t const&,
    parser::expr_t const&,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * Assign a type to a function application in the given environment and context.
 *
 * @param loc   The location in the source file where the expression was found.
 *              If type-assignment fails, it will be copied in the error message.
 *
 * @param usage Helper object to keep track of usage of variables in the given context.
 *
 * @param usage_multiplier  When adding up usage of variables, their quantity is multiplied by this amount
 *                          in order to account for nested usage. For example, in an erased context this
 *                          multiplier can be `zero` or when checking against an unrestricted function
 *                          argument the multiplier should be `many`.
 *
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t>
type_assign_app(
    environment_t const&,
    context_t const&,
    parser::expr_t::app_t const&,
    source_loc_t const& loc,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * Assign a type to an abstraction in the given environment and context.
 *
 * @param loc   The location in the source file where the expression was found.
 *              If type-assignment fails, it will be copied in the error message.
 * @param name  If the abstraction is given a name, it can call itself recursively.
 *
 * @param usage Helper object to keep track of usage of variables in the given context.
 *
 * @param usage_multiplier  When adding up usage of variables, their quantity is multiplied by this amount
 *                          in order to account for nested usage. For example, in an erased context this
 *                          multiplier can be `zero` or when checking against an unrestricted function
 *                          argument the multiplier should be `many`.
 *
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t>
type_assign_abs(
    environment_t const&,
    context_t const&,
    parser::expr_t::abs_t const&,
    source_loc_t const& loc,
    std::optional<source_text> const& name,
    usage_t& usage,
    ast::qty_t usage_multiplier);

} // namespace dep0::typecheck
