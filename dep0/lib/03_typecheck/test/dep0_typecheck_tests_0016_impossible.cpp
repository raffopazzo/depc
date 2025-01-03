/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_typecheck_tests_0016_impossible
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;
using enum dep0::ast::qty_t;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0016_impossible, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "absurd");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_typename, "t"));
        BOOST_TEST(is_arg(f->value.args[1], true_t_of(constant(false)), std::nullopt, zero));
        BOOST_TEST(is_var(f->value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_impossible(f->value.body.stmts[0]));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], true_t_of(constant(false)), "p", zero));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("absurd"), is_i32, var("p"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::axiom_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "nothing_smaller_than_false");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_bool, "a"));
        BOOST_TEST(is_arg(f->signature.args[1], true_t_of(lt(var("a"), constant(false))), std::nullopt));
        BOOST_TEST(is_true_t_of(f->signature.ret_type.get(), constant(false)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "a_lt_b_implies_b");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "b"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(var("a"), var("b"))), std::nullopt, zero));
        BOOST_TEST(is_true_t_of(f->value.ret_type.get(), var("b")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0],
                var("b"),
                std::tuple{return_of(init_list_of())},
                std::tuple{
                    if_else_of(
                        lt(var("a"), constant(false)),
                        std::tuple{
                            impossible_of(
                                app_of(
                                    global("nothing_smaller_than_false"),
                                    var("a"),
                                    expr_of_type(is_unnamed_var, true_t_of(lt(var("a"), constant(false))))))},
                        std::tuple{is_impossible})
                }));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::axiom_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "nothing_bigger_than_true");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_bool, "a"));
        BOOST_TEST(is_arg(f->signature.args[1], true_t_of(gt(var("a"), constant(true))), std::nullopt));
        BOOST_TEST(is_true_t_of(f->signature.ret_type.get(), constant(false)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "a_gt_b_implies_a");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "b"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(gt(var("a"), var("b"))), std::nullopt, zero));
        BOOST_TEST(is_true_t_of(f->value.ret_type.get(), var("a")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0],
                var("b"),
                std::tuple{
                    impossible_of(
                        app_of(
                            global("nothing_bigger_than_true"),
                            var("a"),
                            expr_of_type(is_unnamed_var, true_t_of(gt(var("a"), constant(true))))))},
                std::tuple{
                    if_else_of(
                        var("a"),
                        std::tuple{return_of(init_list_of())},
                        std::tuple{is_impossible})
                }));
    }
}

BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0016_impossible/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0016_impossible/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0016_impossible/pass_005.depc")); }

BOOST_AUTO_TEST_SUITE_END()
