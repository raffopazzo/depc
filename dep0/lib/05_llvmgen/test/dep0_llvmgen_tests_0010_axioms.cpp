/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_llvmgen_tests_0010_axioms
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;
using enum llvm::CmpInst::Predicate;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0010_axioms, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0010_axioms/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i64, "i", zext), arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const i = f->getArg(0);
        auto const xs = f->getArg(1);
        BOOST_TEST(
            is_branch_of(
                entry->getTerminator(),
                cmp_of(llvm::CmpInst::Predicate::ICMP_UGE, exactly(i), constant(3)),
                exactly(then0),
                exactly(else0)));
        BOOST_TEST(
            is_return_of(then0->getTerminator(), constant(-1)));
        BOOST_TEST(
            is_return_of(
                else0->getTerminator(),
                load_of(
                    is_i32,
                    gep_of(is_i32, exactly(xs), exactly(i)),
                    align_of(4))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0010_axioms/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i64, "i", zext), arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const i = f->getArg(0);
        auto const xs = f->getArg(1);
        BOOST_TEST(
            is_branch_of(
                entry->getTerminator(),
                cmp_of(llvm::CmpInst::Predicate::ICMP_UGE, exactly(i), constant(3)),
                exactly(then0),
                exactly(else0)));
        BOOST_TEST(
            is_return_of(then0->getTerminator(), constant(-1)));
        BOOST_TEST(
            is_return_of(
                else0->getTerminator(),
                load_of(
                    is_i32,
                    gep_of(is_i32, exactly(xs), exactly(i)),
                    align_of(4))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0010_axioms/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i64, "i", zext), arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 4ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const next0 = blks[3];
        auto const i = f->getArg(0);
        auto const xs = f->getArg(1);
        BOOST_TEST(
            is_branch_of(
                entry->getTerminator(),
                cmp_of(llvm::CmpInst::Predicate::ICMP_UGE, exactly(i), constant(3)),
                exactly(then0),
                exactly(else0)));
        BOOST_TEST(is_return_of(then0->getTerminator(), constant(-1)));
        BOOST_TEST(is_unconditional_branch_to(else0->getTerminator(), exactly(next0)));
        BOOST_TEST(
            is_return_of(
                next0->getTerminator(),
                load_of(
                    is_i32,
                    gep_of(is_i32, exactly(xs), exactly(i)),
                    align_of(4))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST_REQUIRE(pass("0010_axioms/pass_003.depc")); }

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)

BOOST_AUTO_TEST_SUITE_END()

