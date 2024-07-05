#include "ast_tests_fixture.hpp"

#include "dep0/testing/literal_string.hpp"

namespace dep0::ast {

AstTestsFixture::func_def_t
AstTestsFixture::func_def(char const* const name, std::vector<func_arg_t> args, expr_t ret_type, body_t body)
{
    return func_def(is_mutable_t::no, name, std::move(args), std::move(ret_type), std::move(body));
}

AstTestsFixture::func_def_t
AstTestsFixture::func_def(
    is_mutable_t const m,
    char const* const name,
    std::vector<func_arg_t> args,
    expr_t ret_type,
    body_t body)
{
    return func_def_t{
        dummy_properties_t{},
        testing::literal_string(name),
        expr_t::abs_t{m, std::move(args), std::move(ret_type), std::move(body)}
    };
}

AstTestsFixture::expr_t AstTestsFixture::typename_() { return expr_t{dummy_properties_t{}, expr_t::typename_t{}}; }
AstTestsFixture::expr_t AstTestsFixture::i32() { return expr_t{dummy_properties_t{}, expr_t::i32_t{}}; }
AstTestsFixture::expr_t AstTestsFixture::u32() { return expr_t{dummy_properties_t{}, expr_t::u32_t{}}; }

AstTestsFixture::body_t AstTestsFixture::body(stmt_t s)
{
    return body_t{dummy_properties_t{}, std::vector{std::move(s)}};
}

AstTestsFixture::stmt_t AstTestsFixture::return_(expr_t e)
{
    return stmt_t{dummy_properties_t{}, stmt_t::return_t{std::move(e)}};
}

AstTestsFixture::expr_t AstTestsFixture::app(expr_t f, std::vector<expr_t> args)
{
    return expr_t{dummy_properties_t{}, expr_t::app_t{std::move(f), std::move(args)}};
}

AstTestsFixture::expr_t AstTestsFixture::numeric_constant(int const value)
{
    return expr_t{dummy_properties_t{}, expr_t::numeric_constant_t{value}};
}

AstTestsFixture::expr_t AstTestsFixture::string_literal(char const* const value)
{
    return expr_t{dummy_properties_t{}, expr_t::string_literal_t{testing::literal_string(value)}};
}

AstTestsFixture::expr_t AstTestsFixture::var(char const* const name)
{
    return expr_t{dummy_properties_t{}, var_t(name)};
}

AstTestsFixture::expr_t::var_t AstTestsFixture::var_t(char const* const name)
{
    return expr_t::var_t{testing::literal_string(name)};
}

AstTestsFixture::func_arg_t AstTestsFixture::arg(expr_t type, char const* const var_name)
{
    return arg(qty_t::many, std::move(type), var_name);
}

AstTestsFixture::func_arg_t AstTestsFixture::arg(qty_t const qty, expr_t type, char const* const var_name)
{
    return func_arg_t{
        dummy_properties_t{},
        qty,
        std::move(type),
        var_name ? std::optional{var_t(var_name)} : std::nullopt
    };
}

AstTestsFixture::expr_t AstTestsFixture::abs(std::vector<func_arg_t> args, expr_t ret_type, body_t body)
{
    return abs(is_mutable_t::no, std::move(args), std::move(ret_type), std::move(body));
}

AstTestsFixture::expr_t
AstTestsFixture::abs(is_mutable_t const m, std::vector<func_arg_t> args, expr_t ret_type, body_t body)
{
    return expr_t{dummy_properties_t{}, expr_t::abs_t{m, std::move(args), std::move(ret_type), std::move(body)}};
}

AstTestsFixture::expr_t AstTestsFixture::pi(std::vector<func_arg_t> args, expr_t ret_type)
{
    return pi(is_mutable_t::no, std::move(args), std::move(ret_type));
}

AstTestsFixture::expr_t AstTestsFixture::pi(is_mutable_t const m, std::vector<func_arg_t> args, expr_t ret_type)
{
    return expr_t{dummy_properties_t{}, expr_t::pi_t{m, std::move(args), std::move(ret_type)}};
}

} // namespace dep0::ast
