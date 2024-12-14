/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_llvmgen_tests_0021_tuples
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0021_tuples, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("f1");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_ptr_to(tuple_type),
                    arg_of(is_i32, "x", sext),
                    arg_of(is_i32, "y", zext)},
                is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep1   = inst[0];
        auto const store1 = inst[1];
        auto const gep2   = inst[2];
        auto const store2 = inst[3];
        auto const ret    = inst[4];
        BOOST_TEST(is_gep_of(gep1, tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0)));
        BOOST_TEST(is_gep_of(gep2, tuple_type, exactly(f->getArg(0ul)), constant(0), constant(1)));
        BOOST_TEST(is_store_of(store1, is_i32, exactly(f->getArg(1ul)), exactly(gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, exactly(f->getArg(2ul)), exactly(gep2), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        auto const tuple_type = struct_of(is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 3ul);
        auto const gep1   = inst[0];
        auto const store1 = inst[1];
        auto const ret    = inst[2];
        BOOST_TEST(is_gep_of(gep1, tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(0), exactly(gep1), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 2ul);
        BOOST_TEST(
            is_direct_call(
                inst[0],
                exactly(pass_result.value()->getFunction("f1")),
                call_arg(exactly(f->getArg(0ul))),
                call_arg(constant(0), {sext}),
                call_arg(constant(1), {zext})));
        BOOST_TEST(is_return_of_void(inst[1]));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 10ul);
        auto const gep_array_0      = inst[0];
        auto const gep_tuple_0      = inst[1];
        auto const store1           = inst[2];
        auto const gep_tuple_1      = inst[3];
        auto const store2           = inst[4];
        auto const gep_array_1      = inst[5];
        auto const call1            = inst[6];
        auto const gep_array_2      = inst[7];
        auto const call2            = inst[8];
        auto const ret              = inst[9];
        BOOST_TEST(is_gep_of(gep_array_0, tuple_type, exactly(f->getArg(0ul)), constant(0)));
        BOOST_TEST(is_gep_of(gep_array_1, tuple_type, exactly(f->getArg(0ul)), constant(1)));
        BOOST_TEST(is_gep_of(gep_array_2, tuple_type, exactly(f->getArg(0ul)), constant(2)));
        BOOST_TEST(is_gep_of(gep_tuple_0, tuple_type, exactly(gep_array_0), constant(0), constant(0)));
        BOOST_TEST(is_gep_of(gep_tuple_1, tuple_type, exactly(gep_array_0), constant(0), constant(1)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(0), exactly(gep_tuple_0), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(1), exactly(gep_tuple_1), align_of(4)));
        BOOST_TEST(
            is_direct_call(
                call1,
                exactly(pass_result.value()->getFunction("f1")),
                call_arg(exactly(gep_array_1)),
                call_arg(constant(2), {sext}),
                call_arg(constant(3), {zext})));
        BOOST_TEST(
            is_direct_call(
                call2,
                exactly(pass_result.value()->getFunction("f3")),
                call_arg(exactly(gep_array_2))));

        BOOST_TEST(is_return_of_void(ret));
    }
}

//BOOST_AUTO_TEST_CASE(pass_001)
//{
//    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_001.depc"));
//}

BOOST_AUTO_TEST_SUITE_END()
