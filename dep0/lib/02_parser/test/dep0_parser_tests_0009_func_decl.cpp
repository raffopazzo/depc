/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_parser_tests_0009_func_decl
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0009_func_decl, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 18ul); // 9 forward declarations + 9 definitions
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_arg(f->signature.args[1ul], is_u32, std::nullopt));
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g1");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_typename(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g2");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_typename(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g3");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_arg(f->signature.args[1ul], is_u32, std::nullopt));
        BOOST_TEST(is_typename(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[6ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h1");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), var("g1")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[7ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h2");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, "x"));
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), var("g2"), var("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[8ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h3");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, "x"));
        BOOST_TEST(is_arg(f->signature.args[1ul], is_u32, "y"));
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), var("g3"), var("x"), var("y")));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 5ul); // 2 forward declarations + 3 definitions
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_i32, std::nullopt));
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0],
                app_of(var("f"), var("x")),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_bool)}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_i32, "x"));
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), var("g"), var("x")));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul); // 1 forward declaration + 2 definitions
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(var("f"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_001)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/typecheck_error_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_i32, std::nullopt));
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
}

BOOST_AUTO_TEST_SUITE_END()

