/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_llvmgen_tests_0014_extern_decl
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0014_extern_decl, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_000.depc"));
    auto const f0 = pass_result.value()->getFunction("f0");
    auto const f1 = pass_result.value()->getFunction("f1");
    auto const g = pass_result.value()->getFunction("g");
    BOOST_TEST_REQUIRE(is_function_of(f0, std::tuple{}, is_i32, sext));
    BOOST_TEST_REQUIRE(
        is_function_of(
            f1,
            std::tuple{
                arg_of(is_i32, std::nullopt, sext),
                arg_of(pointer_to(is_i8), std::nullopt, nonnull)},
            is_i32, sext));
    BOOST_TEST_REQUIRE(is_function_of(g, std::tuple{}, is_i32, sext));
    BOOST_TEST_REQUIRE(g->size() == 1ul);
    BOOST_TEST(
        is_return_of(
            g->getEntryBlock().getTerminator(),
            add_of(
                direct_call_of(exactly(f0)),
                direct_call_of(
                    exactly(f1),
                    call_arg(constant(1), sext),
                    call_arg(const_expr_of(gep_of(array_of(2, is_i8), global_of(cstr("2")), constant(0))))))));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("rand");
        BOOST_TEST(is_function_of(f, std::tuple{}, is_i32, sext));
    }
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("always_true");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(fnptr_type(std::tuple{}, is_i32), "f")},
                is_i1, zext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    llvm::CmpInst::Predicate::ICMP_EQ,
                    indirect_call_of(exactly(f->getArg(0))),
                    indirect_call_of(exactly(f->getArg(0))))));
    }
    {
        auto const f = pass_result.value()->getFunction("maybe_true");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(fnptr_type(std::tuple{}, is_i32), "f")},
                is_i1, zext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    llvm::CmpInst::Predicate::ICMP_EQ,
                    indirect_call_of(exactly(f->getArg(0))),
                    indirect_call_of(exactly(f->getArg(0))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("always_true")),
                    call_arg(exactly(pass_result.value()->getFunction("zero"))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("maybe_true")),
                    call_arg(exactly(pass_result.value()->getFunction("rand"))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_002.depc"));
    auto const puts = pass_result.value()->getFunction("puts");
    auto const debug_msg = const_expr_of(gep_of(array_of(10, is_i8), global_of(cstr("debug msg")), constant(0)));
    {
        auto const f = pass_result.value()->getFunction("debug");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 2ul);
        BOOST_TEST(is_direct_call(inst[0], exactly(puts), call_arg(debug_msg)));
        BOOST_TEST(is_return_of(inst[1], constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(exactly(pass_result.value()->getFunction("debug")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_003.depc"));
    auto const puts = pass_result.value()->getFunction("puts");
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = exactly(f->getArg(0));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(x, x)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i8), "s", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(f->getEntryBlock().getTerminator(),
            direct_call_of(
                exactly(pass_result.value()->getFunction("f")),
                call_arg(direct_call_of(exactly(puts), call_arg(exactly(f->getArg(0)))), sext))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_004.depc"));
    BOOST_TEST(pass_result.value()->getFunction("f") == nullptr);
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(4)));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("rand");
        BOOST_TEST(is_function_of(f, std::tuple{}, is_i32, sext));
    }
    {
        auto const f = pass_result.value()->getFunction("sum");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", sext),
                    arg_of(is_i32, "y", sext)},
                is_i32, sext));
        auto const x = exactly(f->getArg(0));
        auto const y = exactly(f->getArg(1));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(x, y)));
    }
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const rand = pass_result.value()->getFunction("rand");
        auto const sum  = pass_result.value()->getFunction("sum");
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 4ul);
        BOOST_TEST(is_direct_call(inst[0], exactly(rand)));
        BOOST_TEST(is_direct_call(inst[1], exactly(rand)));
        BOOST_TEST(
            is_direct_call(
                inst[2],
                exactly(sum),
                call_arg(exactly(inst[0]), sext),
                call_arg(exactly(inst[1]), sext)));
        BOOST_TEST(is_return_of(inst[3], constant(0)));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)
// BOOST_AUTO_TEST_CASE(typecheck_error_003)
// BOOST_AUTO_TEST_CASE(typecheck_error_004)
// BOOST_AUTO_TEST_CASE(typecheck_error_005)
// BOOST_AUTO_TEST_CASE(typecheck_error_006)

BOOST_AUTO_TEST_SUITE_END()
