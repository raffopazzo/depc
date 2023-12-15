#define BOOST_TEST_MODULE dep0_parser_tests_0007_arrays
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0007_arrays, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "values");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], init_list_of(constant(1), constant(2), constant(3))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "empty_of");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_typename, "t"));
        BOOST_TEST(is_array_of(f.value.ret_type.get(), var("t"), constant(0)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], init_list_of()));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "empty_of_int");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_i32, constant(0)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("empty_of"), is_i32)));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "empty_of_bool");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_bool, constant(0)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("empty_of"), is_bool)));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "invalid_length");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], array_of(is_i32, constant(-1)), "xs"));
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST(f.value.body.stmts.size() == 0ul);
    }
}

BOOST_AUTO_TEST_SUITE_END()
