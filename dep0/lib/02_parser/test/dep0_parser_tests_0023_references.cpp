/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_parser_tests_0023_references
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0023_references, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 8ul);
    BOOST_TEST(is_struct_def(pass_result->entries[0], "t", struct_field("x", is_i32)));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(is_i32, var("a")), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], deref(var("x"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(is_i32, var("a")), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], because_of(deref(var("x")), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], array_of(ref_of(is_i32, var("a")), constant(1)), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], deref(subscript_of(var("x"), constant(0)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(var("t"), var("a")), "p"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], member_of(deref(var("p")), "x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], array_of(ref_of(var("t"), var("a")), constant(1)), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], member_of(deref(subscript_of(var("x"), constant(0))), "x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(is_i32, var("a")), "p"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(1)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(deref(var("p")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(var("t"), var("a")), "p"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), var("t"), constant(1)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(deref(var("p")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 9ul);
    {
        auto const f = std::get_if<dep0::parser::extern_decl_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "puts");
    }
    {
        auto const f = std::get_if<dep0::parser::func_decl_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("f1")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("f3")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("puts")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("f0")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x", dep0::ast::qty_t::one));
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 6ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(is_i32, var("a")), "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), is_ref, is_i32, var("a")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], is_bool, "which"));
        BOOST_TEST(is_arg(f->value.args[2], ref_of(is_i32, var("a")), "x"));
        BOOST_TEST(is_arg(f->value.args[3], ref_of(is_i32, var("a")), "y"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), is_ref, is_i32, var("a")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0],
                var("which"),
                std::tuple{return_of(var("x"))},
                std::tuple{return_of(var("y"))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    deref(app_of(var("f0"), scopeof("a"), addressof("a"))),
                    deref(app_of(var("f0"), scopeof("a"), addressof("a"))))));
    }
    BOOST_TEST(is_struct_def(pass_result->entries[3], "t", struct_field("x", is_i32)));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(var("t"), var("a")), "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), is_ref, var("t"), var("a")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], var("t"), "a"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                member_of(deref(app_of(var("f3"), scopeof("a"), addressof("a"))), "x")));
    }
}

BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0023_references/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0023_references/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0023_references/pass_005.depc")); }
BOOST_AUTO_TEST_CASE(pass_006) { BOOST_TEST(pass("0023_references/pass_006.depc")); }

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST(f->name == "f");
    BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
    BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0], deref(var("x"))));
}


BOOST_AUTO_TEST_CASE(typecheck_error_001)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
    BOOST_TEST(f->name == "f");
    BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
    BOOST_TEST(is_arg(f->value.args[0], sigma_of(std::tuple{arg_of(is_scope), arg_of(is_i32)}), "x"));
    BOOST_TEST(is_i32(f->value.ret_type.get()));
    BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->value.body.stmts[0], subscript_of(var("x"), constant(1))));
}

BOOST_AUTO_TEST_CASE(typecheck_error_002)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    BOOST_TEST(is_struct_def(pass_result->entries[0], "t", struct_field("a", is_scope)));
}

BOOST_AUTO_TEST_CASE(typecheck_error_003)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("y")));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_004)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    BOOST_TEST(is_struct_def(pass_result->entries[0], "t"));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("t")));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_005)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::axiom_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("h")));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_006)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    BOOST_TEST(is_integer_def(pass_result->entries[0], "t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("t")));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_007)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x", dep0::ast::qty_t::zero));
        BOOST_TEST(is_scope(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], scopeof("x")));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_008)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_008.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, std::nullopt, dep0::ast::qty_t::one));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "x", dep0::ast::qty_t::one));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x", dep0::ast::qty_t::one));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("f0"), scopeof("x"), var("x"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_009)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_009.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::axiom_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_i32, "x"));
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), is_ref, is_i32, scopeof("x")));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_010)
{
    BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_010.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), is_ref, is_i32, scopeof("x")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], addressof("x")));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_011) { BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_011.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_012) { BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_012.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_013) { BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_013.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_014) { BOOST_TEST_REQUIRE(pass("0023_references/typecheck_error_014.depc")); }

BOOST_AUTO_TEST_SUITE_END()
