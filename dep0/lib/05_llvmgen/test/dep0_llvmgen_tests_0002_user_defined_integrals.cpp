/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_llvmgen_tests_0002_user_defined_integrals
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0002_user_defined_integrals, LLVMGenTestsFixture)

// BOOST_AUTO_TEST_CASE(pass_000) -- this test was removed
// BOOST_AUTO_TEST_CASE(pass_001) -- this test was removed
// BOOST_AUTO_TEST_CASE(pass_002) -- this test was removed
// BOOST_AUTO_TEST_CASE(pass_003) -- this test was removed
// BOOST_AUTO_TEST_CASE(pass_004) -- this test was removed

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("min_signed_8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-128)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_signed_16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-32768)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_signed_32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(std::numeric_limits<std::int32_t>::min())));
    }
    {
        auto const f = pass_result.value()->getFunction("min_signed_64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(std::numeric_limits<std::int64_t>::min())));
    }
    {
        auto const f = pass_result.value()->getFunction("max_signed_8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(127)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_signed_16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(32767)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_signed_32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(2147483647)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_signed_64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(9223372036854775807ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_unsigned_8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(255ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_unsigned_16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(65535ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_unsigned_32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(4294967295ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_unsigned_64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(18446744073709551615ul)));
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

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)
// BOOST_AUTO_TEST_CASE(typecheck_error_003)
// BOOST_AUTO_TEST_CASE(typecheck_error_004)
// BOOST_AUTO_TEST_CASE(typecheck_error_005)
// BOOST_AUTO_TEST_CASE(typecheck_error_006)
// BOOST_AUTO_TEST_CASE(typecheck_error_007)
// BOOST_AUTO_TEST_CASE(typecheck_error_008)

BOOST_AUTO_TEST_SUITE_END()
