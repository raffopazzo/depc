/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_parser_tests_0022_structs
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0022_structs, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0022_structs/pass_000.depc"));
    // 3 types + 4 functions
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 7ul);
    BOOST_TEST(is_struct_def(pass_result->entries[0], "t"));
    BOOST_TEST(is_struct_def(pass_result->entries[1], "u", struct_field("x", is_i32), struct_field("y", is_u64)));
    BOOST_TEST(is_struct_def(pass_result->entries[2], "v", struct_field("x", is_i32), struct_field("u", var("u"))));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "u"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(constant(1), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "v"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(constant(2), init_list_of(constant(3), constant(4)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "v"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(constant(2), app_of(var("f1")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0022_structs/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    BOOST_TEST(
        is_struct_def(
            pass_result->entries[0], "t",
            struct_field("n", is_u64),
            struct_field("v", array_of(is_i32, var("n")))));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], var("t"), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                lt(constant(0), member_of(var("x"), "n")),
                std::tuple{return_of(subscript_of(member_of(var("x"), "v"), constant(0)))},
                std::tuple{return_of(constant(0))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0022_structs/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0022_structs/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0022_structs/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0022_structs/pass_005.depc")); }
BOOST_AUTO_TEST_CASE(pass_006) { BOOST_TEST(pass("0022_structs/pass_006.depc")); }
BOOST_AUTO_TEST_CASE(pass_007) { BOOST_TEST(pass("0022_structs/pass_007.depc")); }
BOOST_AUTO_TEST_CASE(pass_008) { BOOST_TEST(pass("0022_structs/pass_008.depc")); }
BOOST_AUTO_TEST_CASE(pass_009) { BOOST_TEST(pass("0022_structs/pass_009.depc")); }

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0022_structs/typecheck_error_000.depc"));
    BOOST_TEST(is_struct_def(pass_result->entries[0], "u", struct_field("x", var("u"))));
}

BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST_REQUIRE(pass("0022_structs/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST_REQUIRE(pass("0022_structs/typecheck_error_002.depc")); }

BOOST_AUTO_TEST_CASE(parse_error_000)
{
    BOOST_TEST_REQUIRE(fail("0022_structs/parse_error_000.depc"));
}

BOOST_AUTO_TEST_SUITE_END()
