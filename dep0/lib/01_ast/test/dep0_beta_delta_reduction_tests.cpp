#define BOOST_TEST_MODULE dep0_beta_delta_reduction_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/ast/beta_delta_reduction.hpp"

#include "dep0/testing/literal_string.hpp"

using namespace dep0::testing;

namespace dep0::ast {

struct dummy_properties_t
{
    using module_properties_type = dummy_properties_t;
    using type_def_properties_type = dummy_properties_t;
    using func_def_properties_type = dummy_properties_t;
    using func_arg_properties_type = dummy_properties_t;
    using type_properties_type = dummy_properties_t;
    using body_properties_type = dummy_properties_t;
    using stmt_properties_type = dummy_properties_t;
    using expr_properties_type = dummy_properties_t;
};
static_assert(Properties<dummy_properties_t>);

struct Fixture
{
    using module_t = dep0::ast::module_t<dummy_properties_t>;
    using type_def_t = dep0::ast::type_def_t<dummy_properties_t>;
    using func_def_t = dep0::ast::func_def_t<dummy_properties_t>;
    using func_arg_t = dep0::ast::func_arg_t<dummy_properties_t>;
    using type_t = dep0::ast::type_t<dummy_properties_t>;
    using body_t = dep0::ast::body_t<dummy_properties_t>;
    using stmt_t = dep0::ast::stmt_t<dummy_properties_t>;
    using expr_t = dep0::ast::expr_t<dummy_properties_t>;
    using sort_t = dep0::ast::sort_t<dummy_properties_t>;
    using context_t = dep0::ast::delta_reduction::context_t<dummy_properties_t>;

    context_t ctx;

    func_def_t func_def(char const* const name, std::vector<expr_t::abs_t::arg_t> args, type_t ret, body_t body)
    {
        return func_def_t{
            dummy_properties_t{},
            literal_string(name),
            expr_t::abs_t{std::move(args), std::move(ret), std::move(body)}
        };
    }

    type_t type(type_t::value_t value) { return type_t{dummy_properties_t{}, std::move(value)}; }
    type_t::var_t type_var(char const* const name) { return type_t::var_t{literal_string(name)}; }
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
            expr_t::numeric_constant_t{std::nullopt, literal_string(number)}};
    }
    expr_t var(char const* const name)
    {
        return expr_t{dummy_properties_t{}, expr_t::var_t{literal_string(name)}};
    }
    func_arg_t arg(func_arg_t::value_t value) { return func_arg_t{dummy_properties_t{}, std::move(value)}; }
};

BOOST_FIXTURE_TEST_SUITE(dep0_beta_delta_reduction_tests, Fixture)

BOOST_AUTO_TEST_CASE(nop_test)
{
    auto f = func_def("f", {}, type(type_t::i32_t{}), body(return_(numeric_constant("0"))));
    BOOST_TEST(beta_delta_normalize(ctx, f) == false);
}

BOOST_AUTO_TEST_CASE(expand_function_definition)
{
    auto f = func_def("f", {}, type(type_t::i32_t{}), body(return_(numeric_constant("0"))));
    auto g = func_def("g", {}, type(type_t::i32_t{}), body(return_(app(var("f"), {}))));
    bool const inserted = ctx.try_emplace(indexed_var_t{literal_string("f")}, f.value).second;
    BOOST_TEST(inserted);
    BOOST_TEST(beta_delta_normalize(ctx, g) == true);
}

BOOST_AUTO_TEST_CASE(type_argument_should_shadow_function_definitions)
{
    auto f = func_def("f", {}, type(type_t::i32_t{}), body(return_(numeric_constant("0"))));
    auto g =
        func_def(
            "g",
            {arg(func_arg_t::type_arg_t{type_var("f")})},
            type(type_t::i32_t{}),
            body(return_(app(var("f"), {}))));
    bool const inserted = ctx.try_emplace(indexed_var_t{literal_string("f")}, f.value).second;
    BOOST_TEST(inserted);
    BOOST_TEST(beta_delta_normalize(ctx, g) == false);
}

BOOST_AUTO_TEST_CASE(term_argument_should_shadow_function_definitions)
{
    auto f = func_def("f", {}, type(type_t::i32_t{}), body(return_(numeric_constant("0"))));
    auto g =
        func_def(
            "g",
            {arg(func_arg_t::term_arg_t{type(type_t::i32_t{}), expr_t::var_t{literal_string("f")}})},
            type(type_t::i32_t{}),
            body(return_(app(var("f"), {}))));
    bool const inserted = ctx.try_emplace(indexed_var_t{literal_string("f")}, f.value).second;
    BOOST_TEST(inserted);
    BOOST_TEST(beta_delta_normalize(ctx, g) == false);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
