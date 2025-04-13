/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_typecheck_tests_0021_tuples
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0021_tuples, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 6ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_sigma_of(f->value.ret_type.get(), std::tuple{}));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_u32, "y"));
        BOOST_TEST(is_sigma_of(f->value.ret_type.get(), std::tuple{arg_of(is_i32), arg_of(is_u32)}));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_sigma_of(f->value.ret_type.get(), std::tuple{arg_of(is_i32)}));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(constant(0))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_sigma_of(f->value.ret_type.get(), std::tuple{arg_of(is_i32, "x"), arg_of(is_u32, "y")}));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("f1"), constant(0), constant(1))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(
            is_array_of(
                f->value.ret_type.get(),
                sigma_of(std::tuple{arg_of(is_i32), arg_of(is_u32)}), constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(
                    init_list_of(constant(0), constant(1)),
                    app_of(global("f1"), constant(2), constant(3)),
                    app_of(global("f3")))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(
            is_sigma_of(
                f->value.ret_type.get(),
                std::tuple{arg_of(is_i32), arg_of(sigma_of(std::tuple{arg_of(is_i16), arg_of(is_i8)}))}));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(constant(7), init_list_of(constant(8), constant(9)))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 6ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], sigma_of(std::tuple{}), std::nullopt));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], sigma_of(std::tuple{arg_of(is_i32), arg_of(is_u32)}), "x"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("x"), constant(1))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], sigma_of(std::tuple{arg_of(is_i32)}), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("x"), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], sigma_of(std::tuple{arg_of(is_i32, "x"), arg_of(is_u32)}), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("x"), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(
            is_arg(
                f->value.args[0],
                array_of(sigma_of(std::tuple{arg_of(is_i32), arg_of(is_u32)}), constant(3)),
                "xs"));
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("f1"), subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(
            is_arg(
                f->value.args[0],
                sigma_of(std::tuple{arg_of(is_i32), arg_of(sigma_of(std::tuple{arg_of(is_i16), arg_of(is_i8)}))}),
                "x"));
        BOOST_TEST(is_i8(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                subscript_of(subscript_of(var("x"), constant(1)), constant(1))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0021_tuples/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0021_tuples/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0021_tuples/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0021_tuples/pass_005.depc")); }
BOOST_AUTO_TEST_CASE(pass_006) { BOOST_TEST(pass("0021_tuples/pass_006.depc")); }
BOOST_AUTO_TEST_CASE(pass_007) { BOOST_TEST(pass("0021_tuples/pass_007.depc")); }
BOOST_AUTO_TEST_CASE(pass_008) { BOOST_TEST(pass("0021_tuples/pass_008.depc")); }

BOOST_AUTO_TEST_CASE(pass_009)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_009.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 4ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], sigma_of(std::tuple{}), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(
            is_arg(
                f->value.args[0],
                sigma_of(
                    std::tuple{
                        arg_of(sigma_of(std::tuple{})),
                        arg_of(sigma_of(std::tuple{}))
                }),
                std::nullopt,
                dep0::ast::qty_t::zero));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("f0"), init_list_of())));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(global("f1"), init_list_of(init_list_of(), init_list_of()))));
    }
}

BOOST_AUTO_TEST_CASE(pass_010) { BOOST_TEST(pass("0021_tuples/pass_010.depc")); }
BOOST_AUTO_TEST_CASE(pass_011) { BOOST_TEST(pass("0021_tuples/pass_011.depc")); }
BOOST_AUTO_TEST_CASE(pass_012) { BOOST_TEST(pass("0021_tuples/pass_012.depc")); }

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(fail("0021_tuples/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(fail("0021_tuples/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(fail("0021_tuples/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST(fail("0021_tuples/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST(fail("0021_tuples/typecheck_error_004.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_005) { BOOST_TEST(fail("0021_tuples/typecheck_error_005.depc")); }

BOOST_AUTO_TEST_SUITE_END()
