#pragma once

#include "dep0/ast/ast.hpp"

#include "dep0/testing/literal_string.hpp"

namespace dep0::ast {

struct dummy_properties_t
{
    using module_properties_type = dummy_properties_t;
    using type_def_properties_type = dummy_properties_t;
    using func_def_properties_type = dummy_properties_t;
    using func_arg_properties_type = dummy_properties_t;
    using body_properties_type = dummy_properties_t;
    using stmt_properties_type = dummy_properties_t;
    using expr_properties_type = dummy_properties_t;
};
static_assert(Properties<dummy_properties_t>);

struct AstTestsFixture
{
    using module_t = dep0::ast::module_t<dummy_properties_t>;
    using type_def_t = dep0::ast::type_def_t<dummy_properties_t>;
    using func_def_t = dep0::ast::func_def_t<dummy_properties_t>;
    using func_arg_t = dep0::ast::func_arg_t<dummy_properties_t>;
    using body_t = dep0::ast::body_t<dummy_properties_t>;
    using stmt_t = dep0::ast::stmt_t<dummy_properties_t>;
    using expr_t = dep0::ast::expr_t<dummy_properties_t>;

    func_def_t func_def(char const* const name, std::vector<expr_t::abs_t::arg_t> args, expr_t ret, body_t body)
    {
        return func_def_t{
            dummy_properties_t{},
            testing::literal_string(name),
            expr_t::abs_t{std::move(args), std::move(ret), std::move(body)}
        };
    }

    expr_t typename_() { return expr_t{dummy_properties_t{}, expr_t::typename_t{}}; }
    expr_t i32() { return expr_t{dummy_properties_t{}, expr_t::i32_t{}}; }
    expr_t u32() { return expr_t{dummy_properties_t{}, expr_t::u32_t{}}; }
    body_t body(stmt_t s) { return body_t{dummy_properties_t{}, std::vector{std::move(s)}}; }
    stmt_t return_(expr_t e) { return stmt_t{dummy_properties_t{}, stmt_t::return_t{std::move(e)}}; }
    expr_t app(expr_t f, std::vector<expr_t> args)
    {
        return expr_t{dummy_properties_t{}, expr_t::app_t{std::move(f), std::move(args)}};
    }
    expr_t numeric_constant(char const* const number)
    {
        return expr_t{
            dummy_properties_t{},
            expr_t::numeric_constant_t{std::nullopt, testing::literal_string(number)}};
    }
    expr_t var(char const* const name)
    {
        return expr_t{dummy_properties_t{}, var_t(name)};
    }
    expr_t::var_t var_t(char const* const name) { return expr_t::var_t{testing::literal_string(name)}; }
    func_arg_t arg(expr_t type, char const* const var_name = nullptr)
    {
        return func_arg_t{
            dummy_properties_t{},
            std::move(type),
            var_name ? std::optional{var_t(var_name)} : std::nullopt
        };
    }
    expr_t abs(std::vector<func_arg_t> args, expr_t ret_type, body_t body)
    {
        return expr_t{dummy_properties_t{}, expr_t::abs_t{std::move(args), std::move(ret_type), std::move(body)}};
    }
    expr_t pi(std::vector<func_arg_t> args, expr_t ret_type)
    {
        return expr_t{dummy_properties_t{}, expr_t::pi_t{std::move(args), std::move(ret_type)}};
    }
};

} // namespace dep0::ast
