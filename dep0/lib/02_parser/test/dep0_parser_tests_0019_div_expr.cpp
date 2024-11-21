/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_parser_tests_0019_div_expr
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0019_div_expr, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "two");
    BOOST_TEST(f->value.args.size() == 0ul);
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant(2), constant(1))));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "minus_two");
    BOOST_TEST(f->value.args.size() == 0ul);
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant(2), constant(-1))));
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "two");
    BOOST_TEST(f->value.args.size() == 0ul);
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant(2), constant(1))));
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 3ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                eq(var("b"), constant(0)),
                std::tuple{return_of(constant(0))}));
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[1ul],
                eq(var("b"), constant(-1)),
                std::tuple{return_of(constant(-1))}));
        BOOST_TEST(is_return_of(f->value.body.stmts[2ul], div(var("a"), var("b"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 3ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                eq(var("b"), constant(0)),
                std::tuple{return_of(constant(0))}));
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[1ul],
                eq(var("a"), constant("-2147483648")),
                std::tuple{return_of(constant(-1))}));
        BOOST_TEST(is_return_of(f->value.body.stmts[2ul], div(var("a"), var("b"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 7ul);
    BOOST_TEST(std::holds_alternative<dep0::parser::axiom_t>(pass_result->entries[0]));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_u32, "c"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(div(var("a"), var("b")), var("c"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_u32, "c"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(var("a"), div(var("b"), var("c")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_u32, "c"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], minus(var("a"), div(var("b"), var("c")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_u32, "c"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], minus(div(var("a"), var("b")), var("c"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_u32, "c"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], mult(div(var("a"), var("b")), var("c"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_u32, "c"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(mult(var("a"), var("b")), var("c"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(var("a"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], true_t_of(neq(var("a"), constant(0))), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant(2), var("a"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "minus_two");
    BOOST_TEST(f->value.args.size() == 0ul);
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant(2), constant(-1))));
}

BOOST_AUTO_TEST_CASE(typecheck_error_001)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/typecheck_error_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(var("a"), var("b"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_002)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/typecheck_error_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "b"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(var("a"), var("b"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_003)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/typecheck_error_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    BOOST_TEST(std::holds_alternative<dep0::parser::axiom_t>(pass_result->entries[0]));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(var("a"), var("b"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_004)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/typecheck_error_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i8(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant("-128"), constant(-1))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_005)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/typecheck_error_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i16(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant("-32768"), constant(-1))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_006)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/typecheck_error_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant("-2147483648"), constant(-1))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_007)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/typecheck_error_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i64(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], div(constant("-9223372036854775808"), constant(-1))));
    }
}

BOOST_AUTO_TEST_SUITE_END()
