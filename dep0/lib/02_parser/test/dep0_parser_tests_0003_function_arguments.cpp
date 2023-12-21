#define BOOST_TEST_MODULE dep0_parser_tests_0003_function_arguments
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0003_function_arguments, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var(("x"))));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("id"), constant(0))));
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("first"), constant(0), constant(1))));
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 2ul);
    BOOST_TEST(is_func_call_of(f.value.body.stmts[0ul], var("first"), constant(0), constant(1)));
    BOOST_TEST(is_return_of(f.value.body.stmts[1ul], constant(0)));
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 5ul);
    {
        auto const& f = pass_result->func_defs[0];
        BOOST_TEST(f.name == "unit");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST(f.value.body.stmts.size() == 0ul);
    }
    {
        auto const& f = pass_result->func_defs[1];
        BOOST_TEST(f.name == "f1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_unit, std::nullopt));
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST(f.value.body.stmts.size() == 0ul);
    }
    {
        auto const& f = pass_result->func_defs[2];
        BOOST_TEST(f.name == "g1");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f1"), app_of(var("unit")))));
    }
    {
        auto const& f = pass_result->func_defs[3];
        BOOST_TEST(f.name == "f2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_unit, "x"));
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[4];
        BOOST_TEST(f.name == "g2");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f2"), app_of(var("unit")))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(pass("0003_function_arguments/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(pass("0003_function_arguments/typecheck_error_001.depc")); }

BOOST_AUTO_TEST_SUITE_END()
