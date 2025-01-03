/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Specializes the generic AST for the parser stage.
 * @see @ref dep0_design_ast
 */
#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

namespace dep0::parser {

/**
 * @brief Trait types required to specialize the AST for the parser stage.
 * 
 * After the parsing stage all we care about is the location in the source code where
 * the AST nodes have been encountered, which is useful to produce descriptive error messages.
 */
struct properties_t
{
    using module_properties_type = source_loc_t;
    using type_def_properties_type = source_loc_t;
    using axiom_properties_type = source_loc_t;
    using extern_decl_properties_type = source_loc_t;
    using func_decl_properties_type = source_loc_t;
    using func_def_properties_type = source_loc_t;
    using func_arg_properties_type = source_loc_t;
    using body_properties_type = source_loc_t;
    using stmt_properties_type = source_loc_t;
    using expr_properties_type = source_loc_t;
};
static_assert(ast::Properties<properties_t>);

using module_t = ast::module_t<properties_t>;
using type_def_t = ast::type_def_t<properties_t>;
using axiom_t = ast::axiom_t<properties_t>;
using extern_decl_t = ast::extern_decl_t<properties_t>;
using func_decl_t = ast::func_decl_t<properties_t>;
using func_def_t = ast::func_def_t<properties_t>;
using func_arg_t = ast::func_arg_t<properties_t>;
using body_t = ast::body_t<properties_t>;
using stmt_t = ast::stmt_t<properties_t>;
using expr_t = ast::expr_t<properties_t>;

} // namespace dep0::parser
