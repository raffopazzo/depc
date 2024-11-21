/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

struct dummy_properties_t
{
    using module_properties_type = dummy_properties_t;
    using type_def_properties_type = dummy_properties_t;
    using axiom_properties_type = dummy_properties_t;
    using extern_decl_properties_type = dummy_properties_t;
    using func_decl_properties_type = dummy_properties_t;
    using func_def_properties_type = dummy_properties_t;
    using func_arg_properties_type = dummy_properties_t;
    using body_properties_type = dummy_properties_t;
    using stmt_properties_type = dummy_properties_t;
    using expr_properties_type = dummy_properties_t;
};
static_assert(Properties<dummy_properties_t>);

struct AstTestsFixture
{
    using properties_t = dummy_properties_t;
    using module_t = dep0::ast::module_t<dummy_properties_t>;
    using type_def_t = dep0::ast::type_def_t<dummy_properties_t>;
    using axiom_t = dep0::ast::axiom_t<dummy_properties_t>;
    using extern_decl_t = dep0::ast::extern_decl_t<dummy_properties_t>;
    using func_decl_t = dep0::ast::func_decl_t<dummy_properties_t>;
    using func_def_t = dep0::ast::func_def_t<dummy_properties_t>;
    using func_arg_t = dep0::ast::func_arg_t<dummy_properties_t>;
    using body_t = dep0::ast::body_t<dummy_properties_t>;
    using stmt_t = dep0::ast::stmt_t<dummy_properties_t>;
    using expr_t = dep0::ast::expr_t<dummy_properties_t>;

    static func_def_t func_def(char const* const name, std::vector<func_arg_t>, expr_t ret_type, body_t);
    static func_def_t func_def(is_mutable_t, char const* const name, std::vector<func_arg_t>, expr_t ret_type, body_t);
    static expr_t typename_();
    static expr_t i32();
    static expr_t u32();
    static body_t body(stmt_t);
    static stmt_t return_(expr_t);
    static expr_t app(expr_t f, std::vector<expr_t> args);
    static expr_t numeric_constant(int);
    static expr_t string_literal(char const*);
    static expr_t var(char const*);
    static expr_t::var_t var_t(char const*);
    static func_arg_t arg(expr_t type, char const* const var_name = nullptr);
    static func_arg_t arg(qty_t, expr_t type, char const* const var_name = nullptr);
    static expr_t abs(std::vector<func_arg_t> args, expr_t ret_type, body_t body);
    static expr_t abs(is_mutable_t, std::vector<func_arg_t> args, expr_t ret_type, body_t body);
    static expr_t pi(std::vector<func_arg_t> args, expr_t ret_type);
    static expr_t pi(is_mutable_t, std::vector<func_arg_t> args, expr_t ret_type);
};

} // namespace dep0::ast
