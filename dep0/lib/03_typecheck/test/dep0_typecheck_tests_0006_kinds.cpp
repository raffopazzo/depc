#define BOOST_TEST_MODULE dep0_typecheck_tests_0006_kinds
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0006_kinds, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "type_identity");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_typename(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], type_expr_of<dep0::typecheck::properties_t>(type_var("t"))));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("x", type_var("t"))));
        BOOST_TEST(is_type_of(f.value.ret_type, type_var("t")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_of(f.value.ret_type, is_type_i32));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("id"), app_of(var("type_identity"), i32), numeric_constant("0"));
        }));
    }
}

BOOST_AUTO_TEST_SUITE_END()
