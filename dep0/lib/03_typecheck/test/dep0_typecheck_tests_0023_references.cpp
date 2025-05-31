/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_typecheck_tests_0023_references
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0023_references, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 8ul);
    BOOST_TEST(is_struct_def(pass_result->entries[0], "t", struct_field("x", is_i32)));
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1]);
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
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2]);
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
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3]);
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
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(global("t"), var("a")), "p"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], member_of(deref(var("p")), "x")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], array_of(ref_of(global("t"), var("a")), constant(1)), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], member_of(deref(subscript_of(var("x"), constant(0))), "x")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[6]);
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
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_scope, "a", dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[1], ref_of(global("t"), var("a")), "p"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), global("t"), constant(1)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(deref(var("p")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0023_references/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0023_references/pass_002.depc")); }

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(fail("0023_references/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(fail("0023_references/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(fail("0023_references/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST(fail("0023_references/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST(fail("0023_references/typecheck_error_004.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_005) { BOOST_TEST(fail("0023_references/typecheck_error_005.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_006) { BOOST_TEST(fail("0023_references/typecheck_error_006.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_007) { BOOST_TEST(fail("0023_references/typecheck_error_007.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_008) { BOOST_TEST(fail("0023_references/typecheck_error_008.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_009) { BOOST_TEST(fail("0023_references/typecheck_error_009.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_010) { BOOST_TEST(fail("0023_references/typecheck_error_010.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_011) { BOOST_TEST(fail("0023_references/typecheck_error_011.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_012) { BOOST_TEST(fail("0023_references/typecheck_error_012.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_013) { BOOST_TEST(fail("0023_references/typecheck_error_013.depc")); }

BOOST_AUTO_TEST_SUITE_END()
