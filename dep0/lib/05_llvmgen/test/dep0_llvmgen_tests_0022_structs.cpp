/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_llvmgen_tests_0022_structs
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0022_structs, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0022_structs/pass_000.depc"));
    auto const t = get_struct("t");
    auto const u = get_struct("u");
    auto const v = get_struct("v");
    BOOST_TEST(is_struct(t, "t"));
    BOOST_TEST(is_struct(u, "u", is_i32, is_i64));
    BOOST_TEST(is_struct(v, "v", is_i32, exactly(u)));
    {
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(exactly(t))}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 1ul);
        BOOST_TEST(is_return_of_void(inst[0]));
    }
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(exactly(u))}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        BOOST_TEST(is_gep_of(inst[0], exactly(u), exactly(f->getArg(0)), constant(0), constant(0)));
        BOOST_TEST(is_store_of(inst[1], is_i32, constant(1), exactly(inst[0]), align_of(4)));
        BOOST_TEST(is_gep_of(inst[2], exactly(u), exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_store_of(inst[3], is_i64, constant(2), exactly(inst[2]), align_of(8)));
        BOOST_TEST(is_return_of_void(inst[4]));
    }
    {
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(exactly(v))}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 8ul);
        BOOST_TEST(is_gep_of(inst[0], exactly(v), exactly(f->getArg(0)), constant(0), constant(0)));
        BOOST_TEST(is_store_of(inst[1], is_i32, constant(2), exactly(inst[0]), align_of(4)));
        BOOST_TEST(is_gep_of(inst[2], exactly(v), exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_gep_of(inst[3], exactly(u), exactly(inst[2]), constant(0), constant(0)));
        BOOST_TEST(is_store_of(inst[4], is_i32, constant(3), exactly(inst[3]), align_of(4)));
        BOOST_TEST(is_gep_of(inst[5], exactly(u), exactly(inst[2]), constant(0), constant(1)));
        BOOST_TEST(is_store_of(inst[6], is_i64, constant(4), exactly(inst[5]), align_of(8)));
        BOOST_TEST(is_return_of_void(inst[7]));
    }
    {
        auto const f = get_function("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(exactly(v))}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        BOOST_TEST(is_gep_of(inst[0], exactly(v), exactly(f->getArg(0)), constant(0), constant(0)));
        BOOST_TEST(is_store_of(inst[1], is_i32, constant(2), exactly(inst[0]), align_of(4)));
        BOOST_TEST(is_gep_of(inst[2], exactly(v), exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_direct_call(inst[3], exactly(get_function("f1")), call_arg(exactly(inst[2]))));
        BOOST_TEST(is_return_of_void(inst[4]));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0022_structs/pass_001.depc"));
    auto const t = get_struct("t");
    BOOST_TEST(is_struct(t, "t", is_i64, pointer_to(is_i32)));
    {
        auto const f = get_function("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), "x", nonnull)}, is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 4ul);
            BOOST_TEST(is_gep_of(inst[0], exactly(t), exactly(f->getArg(0)), constant(0), constant(0)));
            BOOST_TEST(is_load_of(inst[1], is_i64, exactly(inst[0]), align_of(8)));
            BOOST_TEST(is_cmp(inst[2], llvm::CmpInst::Predicate::ICMP_ULT, constant(0), exactly(inst[1])));
            BOOST_TEST(is_branch_of(inst[3], exactly(inst[2]), exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 5ul);
            BOOST_TEST(is_gep_of(inst[0], exactly(t), exactly(f->getArg(0)), constant(0), constant(1)));
            BOOST_TEST(is_load_of(inst[1], pointer_to(is_i32), exactly(inst[0]), align_of(8)));
            BOOST_TEST(is_gep_of(inst[2], is_i32, exactly(inst[1]), constant(0)));
            BOOST_TEST(is_load_of(inst[3], is_i32, exactly(inst[2]), align_of(4)));
            BOOST_TEST(is_return_of(inst[4], exactly(inst[3])));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 1ul);
            BOOST_TEST(is_return_of(inst[0], constant(0)));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
