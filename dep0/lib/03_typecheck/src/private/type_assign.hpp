#pragma once

#include "private/usage.hpp"

#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"

namespace dep0::typecheck {

/**
 * Assign a type to the given expression in the given environment and context.
 * 
 * Not all expressions can be assigned a type when taken in isolation;
 * for example numerical expressions and initializer lists.
 *
 * @param is_mutable_allowed
 *      Specifies whether it is allowed to invoke mutable functions from the current context.
 *
 * @param usage,usage_multiplier
 *      @see usage_t
 * 
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t>
type_assign(
    env_t const&,
    ctx_t const&,
    parser::expr_t const&,
    ast::is_mutable_t is_mutable_allowed,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * Assign a type to a function application in the given environment and context.
 *
 * @param loc
 *      The location in the source file where the expression was found.
 *      If type-assignment fails, it will be copied in the error message.
 *
 * @param is_mutable_allowed
 *      Specifies whether it is allowed to invoke mutable functions from the current context.
 *
 * @param usage,usage_multiplier
 *      @see usage_t
 *
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t>
type_assign_app(
    env_t const&,
    ctx_t const&,
    parser::expr_t::app_t const&,
    source_loc_t const& loc,
    ast::is_mutable_t is_mutable_allowed,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * Assign a type to an abstraction in the given environment and context.
 *
 * @param loc
 *      The location in the source file where the expression was found.
 *      If type-assignment fails, it will be copied in the error message.
 *
 * @param name
 *      If the abstraction is given a name, it can call itself recursively.
 *
 * @param usage,usage_multiplier
 *      @see usage_t
 *
 * @return The new expression with its type assigned in the properties field.
 */
expected<expr_t>
type_assign_abs(
    env_t const&,
    ctx_t const&,
    parser::expr_t::abs_t const&,
    source_loc_t const& loc,
    std::optional<source_text> const& name,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * Try to assign the same type to two expressions,
 * for example if they appear in a binary expression, like `x + 1` or `1 < x`.
 *
 * @param is_mutable_allowed
 *      Specifies whether it is allowed to invoke mutable functions from the current context.
 *
 * @param usage,usage_multiplier
 *      @see usage_t
 *
 * @return
 *      The result of each individual type-assignment; either one or both might have either failed or succeded.
 *      If both succeded the returned expressions will have the same type.
 */
std::pair<expected<expr_t>, expected<expr_t>>
type_assign_pair(
    env_t const&,
    ctx_t const&,
    parser::expr_t const&,
    parser::expr_t const&,
    ast::is_mutable_t const is_mutable_allowed,
    usage_t& usage,
    ast::qty_t const usage_multiplier);

} // namespace dep0::typecheck
