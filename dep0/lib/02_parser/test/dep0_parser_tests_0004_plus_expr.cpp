/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_parser_tests_0004_plus_expr
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0004_plus_expr, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "three");
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(constant(1), constant(2))));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "three");
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(constant(1), constant(2))));
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "three");
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(constant(1), constant(2))));
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "minus_one");
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(constant(1), constant(-2))));
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 13ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "one");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(1)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "two");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(2)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "three");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    app_of(var("one")),
                    app_of(var("two")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "four");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    app_of(var("three")),
                    constant(1))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "five");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    constant(2),
                    app_of(var("three")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "six");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    constant(3),
                    app_of(var("three")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "nine");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    plus(
                        constant(1),
                        app_of(var("six"))),
                    constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "eight");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    constant(-1),
                    app_of(var("nine")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "seven");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    app_of(var("eight")),
                    constant(-1))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "ten");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    app_of(var("seven")),
                    constant(3))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_a");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    plus(
                        plus(
                            constant(-1),
                            app_of(var("one"))),
                        app_of(var("one"))),
                    constant(-1))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_b");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    plus(
                        plus(
                            app_of(var("one")),
                            constant(-1)),
                        app_of(var("one"))),
                    constant(-1))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_c");
        BOOST_TEST(f->value.args.empty());
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    plus(
                        plus(
                            app_of(var("one")),
                            app_of(var("one"))),
                        constant(-1)),
                    constant(-1))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "minus_one");
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(constant(1), constant(-2))));
}

BOOST_AUTO_TEST_SUITE_END()
