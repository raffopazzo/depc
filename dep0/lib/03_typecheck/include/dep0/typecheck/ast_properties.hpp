/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines the trait types required to specialize the AST for the typechecking stage.
 * @see @ref dep0_design_ast
 */
#pragma once

#include "dep0/ast/concepts.hpp"

namespace dep0::typecheck {

/** @cond DEP0_DOXYGEN_HIDE */
struct legal_module_t;
struct legal_type_def_t;
struct legal_axiom_t;
struct legal_extern_decl_t;
struct legal_func_decl_t;
struct legal_func_def_t;
struct legal_func_arg_t;
struct legal_body_t;
struct legal_stmt_t;
struct legal_expr_t;
/** @endcond */

/**
 * @brief Trait types required to specialize the AST for the parser stage.
 *
 * After typechecking succeeds we have obtained a "legal" AST.
 * An expression is "legal" if a type can be assigned to it.
 * In particular, the `properties` field of an `expr_t` contains the type assigned to the expression.
 */
struct properties_t
{
    using module_properties_type = legal_module_t;
    using type_def_properties_type = legal_type_def_t;
    using axiom_properties_type = legal_axiom_t;
    using extern_decl_properties_type = legal_extern_decl_t;
    using func_decl_properties_type = legal_func_decl_t;
    using func_def_properties_type = legal_func_def_t;
    using func_arg_properties_type = legal_func_arg_t;
    using body_properties_type = legal_body_t;
    using stmt_properties_type = legal_stmt_t;
    using expr_properties_type = legal_expr_t;
};
static_assert(ast::Properties<properties_t>);

} // namespace dep0::typecheck
