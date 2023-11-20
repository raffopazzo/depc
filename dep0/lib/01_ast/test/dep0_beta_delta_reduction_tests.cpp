#define BOOST_TEST_MODULE dep0_beta_delta_reduction_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/ast/beta_delta_reduction.hpp"

#include "ast_tests_fixture.hpp"

using namespace dep0::testing;

namespace dep0::ast {

struct Fixture : AstTestsFixture
{
    using context_t = dep0::ast::delta_reduction::context_t<dummy_properties_t>;
    context_t ctx;
};

BOOST_FIXTURE_TEST_SUITE(dep0_beta_delta_reduction_tests, Fixture)

BOOST_AUTO_TEST_CASE(nop_test)
{
    auto f = func_def("f", {}, i32(), body(return_(numeric_constant("0"))));
    BOOST_TEST(beta_delta_normalize(ctx, f) == false);
}

BOOST_AUTO_TEST_CASE(expand_function_definition)
{
    auto f = func_def("f", {}, i32(), body(return_(numeric_constant("0"))));
    auto g = func_def("g", {}, i32(), body(return_(app(var("f"), {}))));
    bool const inserted = ctx.try_emplace(indexed_var_t{literal_string("f")}, f.value).second;
    BOOST_TEST(inserted);
    BOOST_TEST(beta_delta_normalize(ctx, g) == true);
}

BOOST_AUTO_TEST_CASE(type_argument_should_shadow_function_definitions)
{
    auto f = func_def("f", {}, i32(), body(return_(numeric_constant("0"))));
    auto g = func_def("g", {arg(typename_(), "f")}, i32(), body(return_(app(var("f"), {}))));
    bool const inserted = ctx.try_emplace(indexed_var_t{literal_string("f")}, f.value).second;
    BOOST_TEST(inserted);
    BOOST_TEST(beta_delta_normalize(ctx, g) == false);
}

BOOST_AUTO_TEST_CASE(term_argument_should_shadow_function_definitions)
{
    auto f = func_def("f", {}, i32(), body(return_(numeric_constant("0"))));
    auto g = func_def("g", {arg(i32(), "f")}, i32(), body(return_(app(var("f"), {}))));
    bool const inserted = ctx.try_emplace(indexed_var_t{literal_string("f")}, f.value).second;
    BOOST_TEST(inserted);
    BOOST_TEST(beta_delta_normalize(ctx, g) == false);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
