/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_parser_tests_0024_mutability
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0024_mutability, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0024_mutability/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 5ul);
    auto constexpr yes = dep0::ast::is_mutable_t::yes;
    BOOST_TEST(is_struct_def(pass_result->entries[0], "t", struct_field("a", is_i32), struct_field("b", is_i64)));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x", yes));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
        BOOST_TEST(is_assign(f->value.body.stmts[0ul], var("x"), plus(var("x"), constant(1))));
        BOOST_TEST(is_return_of(f->value.body.stmts[1ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs", yes));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
        BOOST_TEST(
            is_assign(
                f->value.body.stmts[0ul],
                subscript_of(var("xs"), constant(0)),
                subscript_of(var("xs"), constant(1))));
        BOOST_TEST(is_return_of(f->value.body.stmts[1ul], var("xs")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], var("t"), "x", yes));
        BOOST_TEST(is_var(f->value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 3ul);
        BOOST_TEST(is_assign(f->value.body.stmts[0ul], member_of(var("x"), "a"), constant(0)));
        BOOST_TEST(is_assign(f->value.body.stmts[1ul], member_of(var("x"), "b"), constant(1)));
        BOOST_TEST(is_return_of(f->value.body.stmts[2ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], sigma_of(std::tuple{arg_of(is_i32), arg_of(is_i64)}), "x", yes));
        BOOST_TEST(is_i64(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 3ul);
        BOOST_TEST(is_assign(f->value.body.stmts[0ul], subscript_of(var("x"), constant(0)), constant(0)));
        BOOST_TEST(is_assign(f->value.body.stmts[1ul], subscript_of(var("x"), constant(1)), constant(1)));
        BOOST_TEST(is_return_of(f->value.body.stmts[2ul], constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0024_mutability/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto constexpr yes = dep0::ast::is_mutable_t::yes;
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n", yes));
        BOOST_TEST(is_arg(f->value.args[1], true_t_of(lt(constant(0), var("n"))), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[2], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("xs"), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0024_mutability/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto constexpr yes = dep0::ast::is_mutable_t::yes;
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n", yes));
        BOOST_TEST(is_arg(f->value.args[1], true_t_of(lt(constant(0), var("n"))), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[2], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
        BOOST_TEST(is_assign(f->value.body.stmts[0ul], var("n"), constant(0)));
        BOOST_TEST(is_return_of(f->value.body.stmts[1ul], subscript_of(var("xs"), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0024_mutability/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0024_mutability/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0024_mutability/pass_005.depc")); }
BOOST_AUTO_TEST_CASE(pass_006) { BOOST_TEST(pass("0024_mutability/pass_006.depc")); }

BOOST_AUTO_TEST_CASE(pass_007)
{
    BOOST_TEST(pass("0024_mutability/pass_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(is_i32, var("a")), "p"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], deref(var("p"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x", dep0::ast::is_mutable_t::yes));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_immutable_block(
                f->value.body.stmts[0ul],
                {"x"},
                std::tuple{
                    return_of(app_of(var("f0"), scopeof("x"), addressof("x")))
                }));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x", dep0::ast::is_mutable_t::yes));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "y", dep0::ast::is_mutable_t::yes));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_immutable_block(
                f->value.body.stmts[0ul],
                {"x", "y"},
                std::tuple{
                    return_of(
                        plus(
                            app_of(var("f0"), scopeof("x"), addressof("x")),
                            app_of(var("f0"), scopeof("y"), addressof("y"))))
                }));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0024_mutability/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "f");
    BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
    BOOST_TEST(is_arg(f->value.args[0], is_i32, "x", dep0::ast::is_mutable_t::no));
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
    BOOST_TEST(is_assign(f->value.body.stmts[0ul], var("x"), plus(var("x"), constant(1))));
    BOOST_TEST(is_return_of(f->value.body.stmts[1ul], var("x")));
}

BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(pass("0024_mutability/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(pass("0024_mutability/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST(pass("0024_mutability/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST(pass("0024_mutability/typecheck_error_004.depc")); }

BOOST_AUTO_TEST_CASE(typecheck_error_005)
{
    BOOST_TEST_REQUIRE(pass("0024_mutability/typecheck_error_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto constexpr yes = dep0::ast::is_mutable_t::yes;
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->name == "f");
    BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
    BOOST_TEST(is_arg(f->value.args[0], is_u64, "n", yes));
    BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, var("n")), "xs"));
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
    BOOST_TEST(is_assign(f->value.body.stmts[0ul], var("n"), constant(1)));
    BOOST_TEST(
        is_if_else(
            f->value.body.stmts[1ul],
            lt(constant(0), var("n")),
            std::tuple{return_of(subscript_of(var("xs"), constant(0)))},
            std::tuple{return_of(constant(0))}));
}

BOOST_AUTO_TEST_CASE(typecheck_error_006) { BOOST_TEST(pass("0024_mutability/typecheck_error_006.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_007) { BOOST_TEST(pass("0024_mutability/typecheck_error_007.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_008) { BOOST_TEST(pass("0024_mutability/typecheck_error_008.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_009) { BOOST_TEST(pass("0024_mutability/typecheck_error_009.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_010) { BOOST_TEST(pass("0024_mutability/typecheck_error_010.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_011) { BOOST_TEST(pass("0024_mutability/typecheck_error_011.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_012) { BOOST_TEST(pass("0024_mutability/typecheck_error_012.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_013) { BOOST_TEST(pass("0024_mutability/typecheck_error_013.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_014) { BOOST_TEST(pass("0024_mutability/typecheck_error_014.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_015) { BOOST_TEST(pass("0024_mutability/typecheck_error_015.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_016) { BOOST_TEST(pass("0024_mutability/typecheck_error_016.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_017) { BOOST_TEST(pass("0024_mutability/typecheck_error_017.depc")); }

BOOST_AUTO_TEST_SUITE_END()
