/**
 * @file check.hpp
 * @brief Family of functions to perform type-checking of expressions obtained from the parsing stage.
 */

#pragma once

#include "proof_state.hpp" // also in `private/`

#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/parser/ast.hpp"

#include <optional>
#include <vector>

namespace dep0::typecheck {

/**
 * Checks whether a type definition is legal;
 * if it is, the type is stored in the given context.
 *
 * @return A legal type definition or an error.
 */
expected<type_def_t> check_type_def(context_t&, parser::type_def_t const&);

/**
 * Checks whether a function definition is legal;
 * if it is, the function is stored in the given context.
 *
 * @return A legal function definition or an error.
 */
expected<func_def_t> check_func_def(context_t&, parser::func_def_t const&);

/**
 * Checks whether a body is legal, i.e. if all its statements are legal and it returns from all paths;
 * the proof state can be modified by rewrite rules.
 *
 * @return A legal body or an error.
 */
expected<body_t> check_body(proof_state_t&, parser::body_t const&);

/**
 * Checks whether a statement is legal, i.e. if all its expressions and bodies are legal;
 * the proof state can be modified by rewrite rules.
 *
 * @return A legal statement or an error.
 */
expected<stmt_t> check_stmt(proof_state_t&, parser::stmt_t const&);

/**
 * Checks whether the given expression is of sort type or kind in the given context.
 *
 * @return A legal type or kind, otherwise an error.
 */
expected<expr_t> check_type(context_t const&, parser::expr_t const&);

/**
 * Checks whether the given expression has the expected type/kind, in the given context.
 *
 * @param expected_type The type/kind that the expression must have.
 *
 * @return A legal expression or an error.
 */
expected<expr_t> check_expr(context_t const&, parser::expr_t const&, sort_t const& expected_type);

/**
 * Checks whether a numerical expression, eg `42`, has the expected type in the given context.
 *
 * @param loc           The location in the source file where the expression was found.
 *                      If type-checking fails, it will be copied in the error message.
 * @param expected_type The type expected for the expression.
 *
 * @return A legal numerical expression or an error.
 */
expected<expr_t> check_numeric_expr(
    context_t const&,
    parser::expr_t::numeric_constant_t const&,
    source_loc_t const& loc,
    expr_t const& expected_type);

/**
 * Checks whether the given function arguments and return type form a legal Pi-type.
 *
 * @param ctx    The context in which type-checking will be performed;
 *               it will be modified to store the expressions introduced by the function arguments.
 * @param args   The arguments of the Pi-type.
 * @param ret_ty The return type of the Pi-type.
 *
 * @return A legal Pi-type or an error.
 */
expected<expr_t> check_pi_type(
    context_t& ctx,
    std::vector<parser::func_arg_t> const& args,
    parser::expr_t const& ret_ty);

} // namespace dep0::typecheck
