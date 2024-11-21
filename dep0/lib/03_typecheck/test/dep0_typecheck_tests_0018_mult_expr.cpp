/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_typecheck_tests_0018_mult_expr
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0018_mult_expr, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0018_mult_expr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "two");
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], mult(constant(1), constant(2))));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0018_mult_expr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "minus_two");
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], mult(constant(1), constant(-2))));
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0018_mult_expr/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "two");
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0ul], mult(constant(1), constant(2))));
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0018_mult_expr/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 4ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_i32, "c"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(mult(var("a"), var("b")), var("c"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_i32, "c"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(var("a"), mult(var("b"), var("c")))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_i32, "c"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], minus(var("a"), mult(var("b"), var("c")))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_arg(f->value.args[2], is_i32, "c"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], minus(mult(var("a"), var("b")), var("c"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(fail("0018_mult_expr/typecheck_error_000.depc"));
    BOOST_TEST(fail_result->error == "numeric constant does not fit inside `u32_t`");
    BOOST_TEST(fail_result->location.has_value());
}

BOOST_AUTO_TEST_SUITE_END()
