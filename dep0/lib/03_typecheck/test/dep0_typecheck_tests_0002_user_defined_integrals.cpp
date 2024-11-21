/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_typecheck_tests_0002_user_defined_integrals
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

using enum dep0::ast::sign_t;
using enum dep0::ast::width_t;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0002_user_defined_integrals, TypecheckTestsFixture)

// BOOST_AUTO_TEST_CASE(pass_000) -- this test was removed
// BOOST_AUTO_TEST_CASE(pass_001) -- this test was removed
// BOOST_AUTO_TEST_CASE(pass_002) -- this test was removed
// BOOST_AUTO_TEST_CASE(pass_003) -- this test was removed
// BOOST_AUTO_TEST_CASE(pass_004) -- this test was removed

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_005.depc"));
    // 8 type defs + 12 func defs
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 20ul);
    auto const signed_8 = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[0]);
    auto const signed_16 = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[1]);
    auto const signed_32 = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[2]);
    auto const signed_64 = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[3]);
    auto const unsigned_8 = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[4]);
    auto const unsigned_16 = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[5]);
    auto const unsigned_32 = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[6]);
    auto const unsigned_64 = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[7]);

    BOOST_TEST_REQUIRE(signed_8);
    BOOST_TEST_REQUIRE(signed_16);
    BOOST_TEST_REQUIRE(signed_32);
    BOOST_TEST_REQUIRE(signed_64);
    BOOST_TEST_REQUIRE(unsigned_8);
    BOOST_TEST_REQUIRE(unsigned_16);
    BOOST_TEST_REQUIRE(unsigned_32);
    BOOST_TEST_REQUIRE(unsigned_64);
    BOOST_TEST(is_integer_def(*signed_8, "signed_8_t", signed_v, _8));
    BOOST_TEST(is_integer_def(*signed_16, "signed_16_t", signed_v, _16));
    BOOST_TEST(is_integer_def(*signed_32, "signed_32_t", signed_v, _32));
    BOOST_TEST(is_integer_def(*signed_64, "signed_64_t", signed_v, _64));
    BOOST_TEST(is_integer_def(*unsigned_8, "unsigned_8_t", unsigned_v, _8));
    BOOST_TEST(is_integer_def(*unsigned_16, "unsigned_16_t", unsigned_v, _16));
    BOOST_TEST(is_integer_def(*unsigned_32, "unsigned_32_t", unsigned_v, _32));
    BOOST_TEST(is_integer_def(*unsigned_64, "unsigned_64_t", unsigned_v, _64));
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[8]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_signed_8");
        BOOST_TEST(is_global(f->value.ret_type.get(), "signed_8_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(-128)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[9]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_signed_16");
        BOOST_TEST(is_global(f->value.ret_type.get(), "signed_16_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(-32768)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[10]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_signed_32");
        BOOST_TEST(is_global(f->value.ret_type.get(), "signed_32_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant("-2147483648")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[11]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_signed_64");
        BOOST_TEST(is_global(f->value.ret_type.get(), "signed_64_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant("-9223372036854775808")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[12]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_signed_8");
        BOOST_TEST(is_global(f->value.ret_type.get(), "signed_8_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(127)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[13]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_signed_16");
        BOOST_TEST(is_global(f->value.ret_type.get(), "signed_16_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(32767)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[14]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_signed_32");
        BOOST_TEST(is_global(f->value.ret_type.get(), "signed_32_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(2147483647)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[15]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_signed_64");
        BOOST_TEST(is_global(f->value.ret_type.get(), "signed_64_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant("9223372036854775807")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[16]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_unsigned_8");
        BOOST_TEST(is_global(f->value.ret_type.get(), "unsigned_8_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(255)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[17]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_unsigned_16");
        BOOST_TEST(is_global(f->value.ret_type.get(), "unsigned_16_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(65535)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[18]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_unsigned_32");
        BOOST_TEST(is_global(f->value.ret_type.get(), "unsigned_32_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant("4294967295")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[19]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_unsigned_64");
        BOOST_TEST(is_global(f->value.ret_type.get(), "unsigned_64_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant("18446744073709551615")));
    }
}

// BOOST_AUTO_TEST_CASE(parse_error_000)
// BOOST_AUTO_TEST_CASE(parse_error_001)
// BOOST_AUTO_TEST_CASE(parse_error_002)
// BOOST_AUTO_TEST_CASE(parse_error_003)
// BOOST_AUTO_TEST_CASE(parse_error_004)
// BOOST_AUTO_TEST_CASE(parse_error_005)
// BOOST_AUTO_TEST_CASE(parse_error_006)
// BOOST_AUTO_TEST_CASE(parse_error_007)
// BOOST_AUTO_TEST_CASE(parse_error_008)
// BOOST_AUTO_TEST_CASE(parse_error_009)
// BOOST_AUTO_TEST_CASE(parse_error_010)

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_000.depc")); }
// BOOST_AUTO_TEST_CASE(typecheck_error_001) -- this test was removed
// BOOST_AUTO_TEST_CASE(typecheck_error_002) -- this test was removed
// BOOST_AUTO_TEST_CASE(typecheck_error_003) -- this test was removed
// BOOST_AUTO_TEST_CASE(typecheck_error_004) -- this test was removed
// BOOST_AUTO_TEST_CASE(typecheck_error_005) -- this test was removed
// BOOST_AUTO_TEST_CASE(typecheck_error_006) -- this test was removed
// BOOST_AUTO_TEST_CASE(typecheck_error_007) -- this test was removed
// BOOST_AUTO_TEST_CASE(typecheck_error_008) -- this test was removed

BOOST_AUTO_TEST_SUITE_END()
