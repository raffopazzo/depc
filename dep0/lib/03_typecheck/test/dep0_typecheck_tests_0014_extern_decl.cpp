/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_typecheck_tests_0014_extern_decl
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

#include "dep0/testing/ostream.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0014_extern_decl, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::extern_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::extern_decl_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_i32, std::nullopt));
        BOOST_TEST(is_arg(f->signature.args[1], is_cstr, std::nullopt));
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::yes);
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                plus(
                    app_of(global("f0")),
                    app_of(global("f1"), constant(1), string_literal("2")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 6ul);
    {
        auto const f = std::get_if<dep0::typecheck::extern_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "rand");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "always_true");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::no);
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], pi_of(std::tuple{}, is_i32), "f"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], eq(app_of(var("f")), app_of(var("f")))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "maybe_true");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::yes);
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], pi_of(std::tuple{}, is_i32, dep0::ast::is_mutable_t::yes), "f"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], eq(app_of(var("f")), app_of(var("f")))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::no);
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("always_true"), global("zero"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::yes);
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("maybe_true"), global("rand"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::extern_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "puts");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_cstr, std::nullopt));
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "debug");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::yes);
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
        BOOST_TEST(is_func_call_of(f->value.body.stmts[0], global("puts"), string_literal("debug msg")));
        BOOST_TEST(is_return_of(f->value.body.stmts[1], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::yes);
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("debug"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::extern_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "puts");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_cstr, std::nullopt));
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::no);
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], plus(var("x"), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::yes);
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_cstr, "s"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("f"), app_of(global("puts"), var("s")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::no);
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_typename, std::nullopt));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], plus(var("x"), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::no);
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                app_of(global("f"), constant(2), pi_of(std::tuple{}, is_i32, dep0::ast::is_mutable_t::yes))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::extern_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "rand");
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_i32(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "sum");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::no);
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "y"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], plus(var("x"), var("y"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.is_mutable == dep0::ast::is_mutable_t::yes);
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
        BOOST_TEST(is_func_call_of(f->value.body.stmts[0], global("sum"), app_of(global("rand")), app_of(global("rand"))));
        BOOST_TEST(is_return_of(f->value.body.stmts[1], constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST_REQUIRE(fail("0014_extern_decl/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST_REQUIRE(fail("0014_extern_decl/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST_REQUIRE(fail("0014_extern_decl/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST_REQUIRE(fail("0014_extern_decl/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST_REQUIRE(fail("0014_extern_decl/typecheck_error_004.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_005) { BOOST_TEST_REQUIRE(fail("0014_extern_decl/typecheck_error_005.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_006) { BOOST_TEST_REQUIRE(fail("0014_extern_decl/typecheck_error_006.depc")); }

BOOST_AUTO_TEST_SUITE_END()
