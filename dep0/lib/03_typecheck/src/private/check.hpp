/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Private functions used to perform type-checking.
 * @see @ref type_checking
 */
#pragma once

#include "private/proof_state.hpp"
#include "private/usage.hpp"

#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"

#include <optional>
#include <vector>

namespace dep0::typecheck {

/**
 * @brief Checks whether a type definition is legal;
 * if it is, the type is stored in the given environment.
 *
 * @return A legal type definition or an error.
 */
expected<type_def_t> check_type_def(env_t&, parser::type_def_t const&);

/**
 * @brief Checks whether an axiom declaration is legal;
 * if it is, the declaration is stored in the given environment.
 *
 * @return A legal axiom declaration or an error.
 */
expected<axiom_t> check_axiom(env_t&, parser::axiom_t const&);

/**
 * @brief Checks whether an extern function declaration is legal;
 * if it is, the declaration is stored in the given environment.
 *
 * @return A legal extern function declaration or an error.
 */
expected<extern_decl_t> check_extern_decl(env_t&, parser::extern_decl_t const&);

/**
 * @brief Checks whether a function declaration is legal;
 * if it is, the declaration is stored in the given environment.
 *
 * @return A legal function declaration or an error.
 */
expected<func_decl_t> check_func_decl(env_t&, parser::func_decl_t const&);

/**
 * @brief Checks whether a function definition is legal;
 * if it is, the function is stored in the given environment.
 *
 * @return A legal function definition or an error.
 */
expected<func_def_t> check_func_def(env_t&, parser::func_def_t const&);

/**
 * @brief Checks whether a body is legal, i.e. if all its statements are legal and it returns from all paths.
 *
 * The proof state is passed by value because this body cannot refine the proof state of any parent body,
 * but its child statements can refine its proof state.
 * For example, an `if` statement without the `false` branch which returns from all paths of the `true` branch,
 * means that the `if` condition was false once execution gets past the `if` statement;
 * this knowledge can be used to refine the current proof state.
 *
 * @param is_mutable
 *      Specifies whether the given body appears inside a mutable function;
 *      if it does, it is legal to invoke other mutable functions.
 *
 * @param usage,usage_multiplier See @ref `dep0::typecheck::usage_t`.
 *
 * @return A legal body or an error.
 */
expected<body_t>
check_body(
    env_t const&,
    proof_state_t,
    parser::body_t const&,
    ast::is_mutable_t is_mutable,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * @brief Checks whether a statement is legal, i.e. if all its expressions and bodies are legal.
 *
 * This function might refine the proof state of the enclosing body.
 * For example, an `if` statement without the `false` branch which returns from all paths of the `true` branch,
 * means that the `if` condition was false once execution gets past the `if` statement.
 *
 * @param is_mutable
 *      Specifies whether the given statement appears inside a mutable function;
 *      if it does, it is legal to invoke other mutable functions.
 *
 * @param usage,usage_multiplier See @ref `dep0::typecheck::usage_t`.
 *
 * @return A legal statement or an error.
 */
expected<stmt_t>
check_stmt(
    env_t const&,
    proof_state_t&,
    parser::stmt_t const&,
    ast::is_mutable_t is_mutable,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * @brief Checks whether the given expression is of sort type or kind in the given environment and context.
 *
 * @return A legal type or kind, otherwise an error.
 */
expected<expr_t> check_type(env_t const&, ctx_t const&, parser::expr_t const&);

/**
 * @brief Checks whether the given expression has the expected type/kind, in the given environment and context.
 *
 * @param expected_type
 *      The type/kind that the expression must have.
 *
 * @param is_mutable
 *      Specifies whether the given expression appears inside a mutable function;
 *      if it does, it is legal to invoke other mutable functions.
 *
 * @param usage,usage_multiplier See @ref `dep0::typecheck::usage_t`.
 *
 * @return A legal expression or an error.
 */
expected<expr_t>
check_expr(
    env_t const&,
    ctx_t const&,
    parser::expr_t const&,
    sort_t const& expected_type,
    ast::is_mutable_t is_mutable,
    usage_t& usage,
    ast::qty_t usage_multiplier);

/**
 * @brief Checks whether a numerical expression, eg `42`, has the expected type in the given environment and context.
 *
 * @param loc
 *      The location in the source file where the expression was found.
 *      If type-checking fails, it will be copied in the error message.
 *
 * @param expected_type
 *      The type expected for the expression.
 *
 * @return A legal numerical expression or an error.
 */
expected<expr_t> check_numeric_expr(
    env_t const&,
    ctx_t const&,
    parser::expr_t::numeric_constant_t const&,
    source_loc_t const& loc,
    expr_t const& expected_type);

/**
 * @brief Checks whether the given function arguments and return type form a legal Pi-type.
 *
 * @param ctx
 *      The context in which type-checking will be performed;
 *      it will be modified to store the expressions introduced by the function arguments.
 *
 * @param loc
 *      The location in the source file where the expression was found.
 *      If type-checking fails, it will be copied in the error message.
 *
 * @param is_mutable
 *      If the resulting Pi-type is legal, it will be assigned the given mutability modifier.
 *
 * @param args
 *      The arguments of the Pi-type.
 *
 * @param ret_ty
 *      The return type of the Pi-type.
 *
 * @return A legal Pi-type or an error.
 */
expected<expr_t> check_pi_type(
    env_t const&,
    ctx_t& ctx,
    source_loc_t const& loc,
    ast::is_mutable_t is_mutable,
    std::vector<parser::func_arg_t> const& args,
    parser::expr_t const& ret_ty);

/**
 * @brief If `expected_type` is nullptr, tries to type-assign the given arithmetic expression;
 * otherwise checks that it has the given expected type.
 *
 * @param loc
 *      The location in the source file where the expression was found.
 *      If this function fails, it will be copied in the error message.
 *
 * @param is_mutable_allowed
 *      Specifies whether it is allowed to invoke mutable functions from the current context.
 *
 * @param usage,usage_multiplier See @ref `dep0::typecheck::usage_t`.
 */
expected<expr_t> check_or_assign(
    env_t const&,
    ctx_t const&,
    parser::expr_t::arith_expr_t const&,
    source_loc_t const& loc,
    sort_t const* expected_type,
    ast::is_mutable_t const is_mutable_allowed,
    usage_t& usage,
    ast::qty_t const usage_multiplier);


} // namespace dep0::typecheck
