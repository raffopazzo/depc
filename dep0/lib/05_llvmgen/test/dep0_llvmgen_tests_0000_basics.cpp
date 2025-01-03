/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_llvmgen_tests_0000_basics
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0000_basics, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000) { BOOST_TEST(pass("0000_basics/pass_000.depc")); }
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0000_basics/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0000_basics/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0000_basics/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0000_basics/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0000_basics/pass_005.depc")); }
BOOST_AUTO_TEST_CASE(pass_006) { BOOST_TEST(pass("0000_basics/pass_006.depc")); }
BOOST_AUTO_TEST_CASE(pass_007) { BOOST_TEST(pass("0000_basics/pass_007.depc")); }
BOOST_AUTO_TEST_CASE(pass_008) { BOOST_TEST(pass("0000_basics/pass_008.depc")); }
BOOST_AUTO_TEST_CASE(pass_009) { BOOST_TEST(pass("0000_basics/pass_009.depc")); }
BOOST_AUTO_TEST_CASE(pass_010) { BOOST_TEST(pass("0000_basics/pass_010.depc")); }
BOOST_AUTO_TEST_CASE(pass_011) { BOOST_TEST(pass("0000_basics/pass_011.depc")); }
BOOST_AUTO_TEST_CASE(pass_012) { BOOST_TEST(pass("0000_basics/pass_012.depc")); }
BOOST_AUTO_TEST_CASE(pass_013) { BOOST_TEST(pass("0000_basics/pass_013.depc")); }

BOOST_AUTO_TEST_CASE(pass_014)
{
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_014.depc"));
    {
        auto const f = pass_result.value()->getFunction("unit");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), is_zeroinitializer));
    }
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), is_zeroinitializer));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), is_zeroinitializer));
    }
}

BOOST_AUTO_TEST_CASE(pass_015)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_015.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, {sext}));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
}

BOOST_AUTO_TEST_CASE(pass_016)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_016.depc"));
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, {sext}));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
}

BOOST_AUTO_TEST_CASE(pass_017)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_017.depc"));
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "which", {zext})}, is_i32, {sext}));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 1ul);
        BOOST_TEST(is_return_of(inst[0], constant(2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_018)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_018.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, std::nullopt, {zext})}, is_i32, {sext}));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 1ul);
        BOOST_TEST(is_return_of(inst[0], constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, {sext}));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 1ul);
        BOOST_TEST(is_return_of(inst[0], constant(0)));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_001) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_002) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_003) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_004) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_005) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_006) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_007) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_008) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_009) doesn't type check

BOOST_AUTO_TEST_SUITE_END()
