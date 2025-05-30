/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_parser_tests_0008_boolean_expr
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0008_boolean_expr, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 16ul);
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "sign_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_8));
    }
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "hours_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negate");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(constant(false), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lt(
                    lt(constant(true), constant(true)),
                    constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lt(
                    constant(true),
                    lt(constant(true), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lt(
                    app_of(var("negate"), lt(constant(true), app_of(var("f1")))),
                    app_of(var("negate"), lt(app_of(var("f1")), constant(true))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(constant(1), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(var("x"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lt(
                    lt(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lt(
                    subscript_of(var("xs"), constant(0)),
                    lt(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f12");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lt(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 16ul);
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "sign_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_8));
    }
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "hours_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negate");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gt(constant(true), constant(false))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gt(gt(constant(true), constant(true)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gt(constant(true), gt(constant(true), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gt(
                    app_of(var("negate"), gt(constant(true), app_of(var("f1")))),
                    app_of(var("negate"), gt(app_of(var("f1")), constant(true))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gt(constant(1), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gt(var("x"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gt(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gt(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gt(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gt(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gt(
                    gt(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gt(
                    subscript_of(var("xs"), constant(0)),
                    gt(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f12");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gt(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 16ul);
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "sign_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_8));
    }
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "hours_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negate");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lte(constant(false), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lte(lte(constant(true), constant(true)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lte(constant(true), lte(constant(true), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lte(
                    app_of(var("negate"), lte(constant(true), app_of(var("f1")))),
                    app_of(var("negate"), lte(app_of(var("f1")), constant(true))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lte(constant(1), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lte(var("x"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lte(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lte(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lte(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lte(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lte(
                    lte(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lte(
                    subscript_of(var("xs"), constant(0)),
                    lte(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f12");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lte(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_003.depc"));
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "sign_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_8));
    }
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "hours_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negate");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gte(constant(true), constant(false))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gte(gte(constant(true), constant(true)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gte(constant(true), gte(constant(true), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gte(
                    app_of(var("negate"), gte(constant(true), app_of(var("f1")))),
                    app_of(var("negate"), gte(app_of(var("f1")), constant(true))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gte(constant(1), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gte(var("x"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gte(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gte(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gte(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], gte(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gte(
                    gte(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gte(
                    subscript_of(var("xs"), constant(0)),
                    gte(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f12");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                gte(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 13ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negate");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(constant(false))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(not_of(constant(true)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(not_of(constant(true)), not_of(constant(false)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(lt(constant(true), constant(false)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1ul], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(not_of(var("x")), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1ul], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(lt(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1ul], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], lt(not_of(var("x")), not_of(var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(subscript_of(var("xs"), constant(0)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lt(not_of(subscript_of(var("xs"), constant(0))), subscript_of(var("xs"), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                not_of(lt(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                lt(not_of(subscript_of(var("xs"), constant(0))), not_of(subscript_of(var("xs"), constant(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 12ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], and_of(constant(true), constant(false))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], and_of(constant(true), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], and_of(constant(true), not_of(constant(false)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], and_of(not_of(constant(false)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(and_of(constant(false), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], and_of(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(and_of(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], and_of(not_of(var("x")), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                and_of(
                    subscript_of(var("xs"), constant(0)),
                    subscript_of(var("xs"), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                and_of(
                    not_of(subscript_of(var("xs"), constant(0))),
                    subscript_of(var("xs"), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                not_of(
                    and_of(
                        subscript_of(var("xs"), constant(0)),
                        subscript_of(var("xs"), constant(1))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                and_of(
                    not_of(subscript_of(var("xs"), constant(0))),
                    not_of(subscript_of(var("xs"), constant(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 16ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], or_of(constant(true), constant(false))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], or_of(constant(true), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], or_of(constant(true), not_of(constant(false)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], or_of(not_of(constant(false)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(or_of(constant(false), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], or_of(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(or_of(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], or_of(not_of(var("x")), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                or_of(
                    and_of(var("x"), var("y")),
                    and_of(not_of(var("x")), not_of(var("y"))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                and_of(
                    and_of(
                        var("x"),
                        or_of(var("y"), not_of(var("x")))),
                    not_of(var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                or_of(
                    or_of(
                        var("x"),
                        and_of(var("y"), not_of(var("x")))),
                    not_of(var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                or_of(
                    or_of(
                        var("x"),
                        and_of(var("y"), not_of(var("x")))),
                    not_of(var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f12");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                or_of(
                    subscript_of(var("xs"), constant(0)),
                    subscript_of(var("xs"), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f13");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                or_of(
                    not_of(subscript_of(var("xs"), constant(0))),
                    subscript_of(var("xs"), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f14");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                not_of(
                    or_of(
                        subscript_of(var("xs"), constant(0)),
                        subscript_of(var("xs"), constant(1))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f15");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                or_of(
                    not_of(subscript_of(var("xs"), constant(0))),
                    not_of(subscript_of(var("xs"), constant(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 16ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(constant(true), constant(false))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(constant(true), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(constant(true), not_of(constant(false)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(not_of(constant(false)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(neq(constant(false), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(neq(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(not_of(var("x")), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                or_of(
                    and_of(var("x"), var("y")),
                    neq(not_of(var("x")), not_of(var("y"))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                and_of(
                    var("x"),
                    neq(
                        or_of(var("y"), not_of(var("x"))),
                        not_of(var("y"))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                and_of(
                    neq(var("x"), var("y")),
                    neq(not_of(var("x")), not_of(var("y"))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                neq(
                    neq(
                        var("x"),
                        and_of(var("y"), not_of(var("x")))),
                    not_of(var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f12");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                neq(
                    subscript_of(var("xs"), constant(0)),
                    subscript_of(var("xs"), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f13");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                neq(
                    not_of(subscript_of(var("xs"), constant(0))),
                    subscript_of(var("xs"), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f14");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                not_of(
                    neq(
                        subscript_of(var("xs"), constant(0)),
                        subscript_of(var("xs"), constant(1))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f15");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(2)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                neq(
                    not_of(subscript_of(var("xs"), constant(0))),
                    not_of(subscript_of(var("xs"), constant(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_008.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 16ul);
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "sign_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_8));
    }
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "hours_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(constant(false), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(eq(constant(false), constant(false)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(constant(false), eq(constant(false), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                and_of(
                    constant(true),
                    eq(constant(false), lt(constant(false), constant(true))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(lt(constant(false), constant(true)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(constant(1), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(var("x"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                eq(
                    eq(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f12");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                eq(
                    subscript_of(var("xs"), constant(0)),
                    eq(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f13");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                eq(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_009)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_009.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 16ul);
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "sign_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_8));
    }
    {
        auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "hours_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(constant(false), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(eq(constant(true), constant(false)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], eq(constant(true), neq(constant(false), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                and_of(
                    constant(true),
                    neq(constant(false), lt(constant(false), constant(true))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(lt(constant(false), constant(true)), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(constant(1), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(var("x"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f->value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], neq(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                eq(
                    neq(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f12");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                neq(
                    subscript_of(var("xs"), constant(0)),
                    eq(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f13");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                neq(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], not_of(var("x"))));
    }
}

BOOST_AUTO_TEST_SUITE_END()
