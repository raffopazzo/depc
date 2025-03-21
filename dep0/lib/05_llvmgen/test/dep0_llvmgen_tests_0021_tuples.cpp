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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(struct_of())}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 1ul);
        BOOST_TEST(is_return_of_void(inst[0]));
    }
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
        BOOST_TEST(is_direct_call(
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
        BOOST_TEST(is_direct_call(
            call1,
            exactly(pass_result.value()->getFunction("f1")),
            call_arg(exactly(gep_array_1)),
            call_arg(constant(2), {sext}),
            call_arg(constant(3), {zext})));
        BOOST_TEST(is_direct_call(
            call2,
            exactly(pass_result.value()->getFunction("f3")),
            call_arg(exactly(gep_array_2))));

        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        auto const tuple_type = struct_of(is_i32, struct_of(is_i16, is_i8));
        auto const sub_tuple_type = struct_of(is_i16, is_i8);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 8ul);
        auto const gep_tuple_0      = inst[0];
        auto const store0           = inst[1];
        auto const gep_tuple_1      = inst[2];
        auto const gep_tuple_1_0    = inst[3];
        auto const store1_0         = inst[4];
        auto const gep_tuple_1_1    = inst[5];
        auto const store1_1         = inst[6];
        auto const ret              = inst[7];
        BOOST_TEST(is_gep_of(gep_tuple_0, tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0)));
        BOOST_TEST(is_gep_of(gep_tuple_1, tuple_type, exactly(f->getArg(0ul)), constant(0), constant(1)));
        BOOST_TEST(is_gep_of(gep_tuple_1_0, sub_tuple_type, exactly(gep_tuple_1), constant(0), constant(0)));
        BOOST_TEST(is_gep_of(gep_tuple_1_1, sub_tuple_type, exactly(gep_tuple_1), constant(0), constant(1)));
        BOOST_TEST(is_store_of(store0, is_i32, constant(7), exactly(gep_tuple_0), align_of(4)));
        BOOST_TEST(is_store_of(store1_0, is_i16, constant(8), exactly(gep_tuple_1_0), align_of(2)));
        BOOST_TEST(is_store_of(store1_1, is_i8, constant(9), exactly(gep_tuple_1_1), align_of(1)));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(struct_of()), std::nullopt, nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(1)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        auto const tuple_type = struct_of(is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "xs", nonnull)}, is_i32, zext));
        auto const xs = exactly(f->getArg(0ul));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("f1")),
                    call_arg(gep_of(tuple_type, xs, constant(2))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        auto const tuple_type = struct_of(is_i32, struct_of(is_i16, is_i8));
        auto const sub_tuple_type = struct_of(is_i16, is_i8);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i8, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(
                    is_i8,
                    gep_of(
                        sub_tuple_type,
                        gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(1)),
                        constant(0), constant(1)),
                    align_of(1))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_002.depc"));
    auto const attrs = std::vector{llvm::Attribute::Alignment};
    auto const align = llvm::Align(8);
    auto const tuple_type = struct_of(is_i32, is_i16);
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_ptr_to(tuple_type),
                    arg_of(pointer_to(tuple_type), "xs", nonnull),
                    arg_of(is_i64, "i", zext)
                },
                is_void));
        auto const result = exactly(f->getArg(0));
        auto const xs     = exactly(f->getArg(1));
        auto const i      = exactly(f->getArg(2));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() > 2ul);
        BOOST_TEST(is_direct_call(
            inst[inst.size() - 2],
            is_memcpy,
            call_arg(bitcast_of(result, pointer_to(tuple_type), pointer_to(is_i8)), attrs, align),
            call_arg(bitcast_of(gep_of(tuple_type, xs, i), pointer_to(tuple_type), pointer_to(is_i8)), attrs, align),
            call_arg(constant(8)),
            call_arg(constant(false))));
        BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        auto const tuple_type = struct_of(is_i32, is_i16);
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_ptr_to(tuple_type),
                    arg_of(pointer_to(struct_of(tuple_type, tuple_type)), "xs", nonnull),
                    arg_of(is_i1, "which", zext),
                },
                is_void));
        auto const result = exactly(f->getArg(0));
        auto const xs     = exactly(f->getArg(1));
        auto const which  = exactly(f->getArg(2));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        {
            BOOST_TEST_REQUIRE(entry->size() == 1ul);
            BOOST_TEST(is_branch_of(entry->getTerminator(), which, exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() > 2ul);
            BOOST_TEST(is_direct_call(
                inst[inst.size() - 2],
                is_memcpy,
                call_arg(bitcast_of(result, pointer_to(tuple_type), pointer_to(is_i8)), attrs, align),
                call_arg(
                    bitcast_of(
                        gep_of(struct_of(tuple_type, tuple_type), xs, constant(0), constant(0)),
                        pointer_to(tuple_type),
                        pointer_to(is_i8)),
                    attrs, align),
                call_arg(constant(8)),
                call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() > 2ul);
            BOOST_TEST(is_direct_call(
                inst[inst.size() - 2],
                is_memcpy,
                call_arg(bitcast_of(result, pointer_to(tuple_type), pointer_to(is_i8)), attrs, align),
                call_arg(
                    bitcast_of(
                        gep_of(struct_of(tuple_type, tuple_type), xs, constant(0), constant(1)),
                        pointer_to(tuple_type),
                        pointer_to(is_i8)),
                    attrs, align),
                call_arg(constant(8)),
                call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
        }
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        auto const tuple_type = struct_of(is_i32, is_i16);
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_ptr_to(tuple_type),
                    arg_of(pointer_to(tuple_type), "x", nonnull),
                    arg_of(pointer_to(tuple_type), "y", nonnull),
                    arg_of(is_i1, "which", zext),
                },
                is_void));
        auto const result = exactly(f->getArg(0));
        auto const x      = exactly(f->getArg(1));
        auto const y      = exactly(f->getArg(2));
        auto const which  = exactly(f->getArg(3));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        {
            BOOST_TEST_REQUIRE(entry->size() == 1ul);
            BOOST_TEST(is_branch_of(entry->getTerminator(), which, exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() > 2ul);
            BOOST_TEST(is_direct_call(
                inst[inst.size() - 2],
                is_memcpy,
                call_arg(bitcast_of(result, pointer_to(tuple_type), pointer_to(is_i8)), attrs, align),
                call_arg(bitcast_of(x, pointer_to(tuple_type), pointer_to(is_i8)), attrs, align),
                call_arg(constant(8)),
                call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() > 2ul);
            BOOST_TEST(is_direct_call(
                inst[inst.size() - 2],
                is_memcpy,
                call_arg(bitcast_of(result, pointer_to(tuple_type), pointer_to(is_i8)), attrs, align),
                call_arg(bitcast_of(y, pointer_to(tuple_type), pointer_to(is_i8)), attrs, align),
                call_arg(constant(8)),
                call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
        }
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("f1");
        auto const tuple_type = struct_of(pointer_to(is_i32), pointer_to(is_i32));
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_ptr_to(is_i32),
                    arg_of(pointer_to(tuple_type), "xs", nonnull),
                    arg_of(is_i1, "which", zext),
                },
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const result = exactly(f->getArg(0));
        auto const xs = exactly(f->getArg(1));
        auto const which = exactly(f->getArg(2));
        {
            BOOST_TEST_REQUIRE(entry->size() == 1ul);
            BOOST_TEST(is_branch_of(entry->getTerminator(), which, exactly(then0), exactly(else0)));
        }
        auto const attrs = std::vector{llvm::Attribute::Alignment};
        auto const align = llvm::Align(4);
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() > 2ul);
            auto const src = gep_of(tuple_type, xs, constant(0), constant(0));
            auto const ptr = load_of(pointer_to(is_i32), src, align_of(8));
            BOOST_TEST(is_direct_call(
                inst[inst.size() - 2],
                is_memcpy,
                call_arg(bitcast_of(result, pointer_to(is_i32), pointer_to(is_i8)), attrs, align),
                call_arg(bitcast_of(ptr, pointer_to(is_i32), pointer_to(is_i8)), attrs, align),
                call_arg(constant(8)),
                call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() > 2ul);
            auto const src = gep_of(tuple_type, xs, constant(0), constant(1));
            auto const ptr = load_of(pointer_to(is_i32), src, align_of(8));
            BOOST_TEST(is_direct_call(
                inst[inst.size() - 2],
                is_memcpy,
                call_arg(bitcast_of(result, pointer_to(is_i32), pointer_to(is_i8)), attrs, align),
                call_arg(bitcast_of(ptr, pointer_to(is_i32), pointer_to(is_i8)), attrs, align),
                call_arg(constant(8)),
                call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
        }
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        auto const tuple_type = struct_of(pointer_to(is_i32), pointer_to(is_i32));
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = exactly(f->getArg(0));
        auto const zero = constant(0);
        auto const one = constant(1);
        auto const ptr_to_array_0 = gep_of(tuple_type, x, zero, zero);
        auto const ptr_to_array_1 = gep_of(tuple_type, x, zero, one);
        auto const ptr_to_i32_0 = load_of(pointer_to(is_i32), ptr_to_array_0, align_of(8));
        auto const ptr_to_i32_1 = load_of(pointer_to(is_i32), ptr_to_array_1, align_of(8));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(
                    load_of(is_i32, gep_of(is_i32, ptr_to_i32_0, one), align_of(4)),
                    load_of(is_i32, gep_of(is_i32, ptr_to_i32_1, zero), align_of(4)))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_004.depc"));
    auto const tuple_type = struct_of(is_i64, pointer_to(is_i32));
    auto const malloc_fn = pass_result.value()->getFunction("malloc");
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const x = exactly(f->getArg(0));
        BOOST_TEST(
            is_branch_of(
                entry->getTerminator(),
                cmp_of(
                    llvm::CmpInst::ICMP_ULT,
                    constant(2),
                    load_of(is_i64, gep_of(tuple_type, x, constant(0), constant(0)), align_of(8))),
                exactly(then0),
                exactly(else0)));
        BOOST_TEST(
            is_return_of(
                then0->getTerminator(),
                load_of(
                    is_i32,
                    gep_of(
                        is_i32,
                        load_of(
                            pointer_to(is_i32),
                            gep_of(tuple_type, x, constant(0), constant(1)),
                            align_of(8)),
                        constant(2)),
                    align_of(4))));
        BOOST_TEST(is_return_of(else0->getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        // there must be exactly 5 stores:
        // 1,2. to store the size and ptr of the allocated array in the return value
        // 3,4,5. to store the values 1,2,3 in the allocated array
        std::vector<llvm::StoreInst*> stores;
        for (auto& inst: f->getEntryBlock())
            if (auto const q = llvm::dyn_cast<llvm::StoreInst>(&inst))
                stores.push_back(q);
        BOOST_TEST_REQUIRE(stores.size() == 5ul);
        auto const fst = gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0));
        auto const snd = gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(1));
        BOOST_TEST(is_store_of(stores[0], is_i64, constant(3), fst, align_of(8)));
        auto const malloc =
            bitcast_of(
                direct_call_of(exactly(malloc_fn), call_arg(constant(12))),
                pointer_to(is_i8),
                pointer_to(is_i32));
        BOOST_TEST(is_store_of(stores[1], is_i32, constant(1), gep_of(is_i32, malloc, constant(0)), align_of(4)));
        BOOST_TEST(is_store_of(stores[2], is_i32, constant(2), gep_of(is_i32, malloc, constant(1)), align_of(4)));
        BOOST_TEST(is_store_of(stores[3], is_i32, constant(3), gep_of(is_i32, malloc, constant(2)), align_of(4)));
        BOOST_TEST(is_store_of(stores[4], pointer_to(is_i32), malloc, snd, align_of(8)));
    }
    {
        auto const f = pass_result.value()->getFunction("extract");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_ptr_to(is_i32),
                    arg_of(pointer_to(tuple_type), "x", nonnull)
                },
                is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() > 2ul);
        auto const fst = gep_of(tuple_type, exactly(f->getArg(1)), constant(0), constant(0));
        auto const snd = gep_of(tuple_type, exactly(f->getArg(1)), constant(0), constant(1));
        auto const ptr = load_of(pointer_to(is_i32), snd, align_of(8));
        auto const attrs = std::vector{llvm::Attribute::Alignment};
        auto const align = llvm::Align(4);
        BOOST_TEST(is_direct_call(
            inst[inst.size() - 2],
            is_memcpy,
            call_arg(bitcast_of(exactly(f->getArg(0)), pointer_to(is_i32), pointer_to(is_i8)), attrs, align),
            call_arg(bitcast_of(ptr, pointer_to(is_i32), pointer_to(is_i8)), attrs, align),
            call_arg(mul_of(load_of(is_i64, fst, align_of(8)), constant(4))),
            call_arg(constant(false))));
        BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(is_i32)}, is_void));
        // there must be exactly 5 stores:
        // 1,2. to store the size and ptr of the allocated array in the return value
        // 3,4,5. to store the values 1,2,3 in the allocated array
        auto const inst = get_instructions(f->getEntryBlock());
        std::vector<llvm::StoreInst const*> stores;
        for (auto const p: inst)
            if (auto const q = llvm::dyn_cast<llvm::StoreInst>(p))
                stores.push_back(q);
        BOOST_TEST_REQUIRE(stores.size() == 5ul);
        auto const tmp = alloca_of(tuple_type, constant(1), align_of(8));
        auto const fst = gep_of(tuple_type, tmp, constant(0), constant(0));
        auto const snd = gep_of(tuple_type, tmp, constant(0), constant(1));
        auto const alloca = alloca_of(is_i32, constant(3), align_of(4));
        BOOST_TEST(is_store_of(stores[0], is_i64, constant(3), fst, align_of(8)));
        BOOST_TEST(is_store_of(stores[1], is_i32, constant(1), gep_of(is_i32, alloca, constant(0)), align_of(4)));
        BOOST_TEST(is_store_of(stores[2], is_i32, constant(2), gep_of(is_i32, alloca, constant(1)), align_of(4)));
        BOOST_TEST(is_store_of(stores[3], is_i32, constant(3), gep_of(is_i32, alloca, constant(2)), align_of(4)));
        BOOST_TEST(is_store_of(stores[4], pointer_to(is_i32), alloca, snd, align_of(8)));
        // finally a call to extract() before the return of void
        BOOST_TEST(
            is_direct_call(
                inst[inst.size() - 2],
                exactly(pass_result.value()->getFunction("extract")),
                call_arg(exactly(f->getArg(0))),
                call_arg(tmp)));
        BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 7ul);
        auto const result = exactly(f->getArg(0));
        auto const gep1     = inst[0];
        auto const call1    = inst[1];
        auto const gep2     = inst[2];
        auto const call2    = inst[3];
        auto const gep3     = inst[4];
        auto const call3    = inst[5];
        auto const ret      = inst[6];
        BOOST_TEST(is_gep_of(gep1, tuple_type, result, constant(0)));
        BOOST_TEST(is_gep_of(gep2, tuple_type, result, constant(1)));
        BOOST_TEST(is_gep_of(gep3, tuple_type, result, constant(2)));
        BOOST_TEST(is_direct_call(call1, exactly(pass_result.value()->getFunction("f2")), call_arg(exactly(gep1))));
        BOOST_TEST(is_direct_call(call2, exactly(pass_result.value()->getFunction("f2")), call_arg(exactly(gep2))));
        BOOST_TEST(is_direct_call(call3, exactly(pass_result.value()->getFunction("f2")), call_arg(exactly(gep3))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 8ul);
        auto const alloca   = inst[0];
        auto const call_f2  = inst[1];
        auto const call_f1  = inst[2];
        auto const gep      = inst[3];
        auto const load     = inst[4];
        auto const bitcast  = inst[5];
        auto const free     = inst[6];
        auto const ret      = inst[7];
        BOOST_TEST(is_alloca(alloca, tuple_type, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(call_f2, exactly(pass_result.value()->getFunction("f2")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(call_f1, exactly(pass_result.value()->getFunction("f1")), call_arg(exactly(alloca))));
        BOOST_TEST(is_gep_of(gep, tuple_type, exactly(alloca), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(bitcast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(pass_result.value()->getFunction("free")), call_arg(exactly(bitcast))));
        BOOST_TEST(is_return_of(ret, exactly(call_f1)));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const inst = get_instructions(*entry);
        BOOST_TEST_REQUIRE(inst.size() == 9ul);
        auto const alloca   = inst[0];
        auto const call_f2  = inst[1];
        auto const call_f1  = inst[2];
        auto const cmp      = inst[3];
        auto const gep      = inst[4];
        auto const load     = inst[5];
        auto const bitcast  = inst[6];
        auto const free     = inst[7];
        auto const br       = inst[8];
        BOOST_TEST(is_alloca(alloca, tuple_type, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(call_f2, exactly(pass_result.value()->getFunction("f2")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(call_f1, exactly(pass_result.value()->getFunction("f1")), call_arg(exactly(alloca))));
        BOOST_TEST(is_cmp(cmp, llvm::CmpInst::ICMP_EQ, exactly(call_f1), constant(0)));
        BOOST_TEST(is_gep_of(gep, tuple_type, exactly(alloca), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(bitcast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(pass_result.value()->getFunction("free")), call_arg(exactly(bitcast))));
        BOOST_TEST(is_branch_of(br, exactly(cmp), exactly(then0), exactly(else0)));
        BOOST_TEST(is_return_of(then0->getTerminator(), constant(1)));
        BOOST_TEST(is_return_of(else0->getTerminator(), constant(2)));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_ptr_to(tuple_type),
                    arg_of(pointer_to(tuple_type), "x", nonnull)},
                is_void));
        auto const attrs = std::vector{llvm::Attribute::Alignment};
        auto const align = llvm::Align(4);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 16ul);
        BOOST_TEST(is_gep_of(inst[0], tuple_type, exactly(f->getArg(0)), constant(0)));
        BOOST_TEST(is_gep_of(inst[1], tuple_type, exactly(f->getArg(1)), constant(0), constant(0)));
        BOOST_TEST(is_gep_of(inst[2], tuple_type, exactly(inst[0]), constant(0), constant(0)));
        BOOST_TEST(is_load_of(inst[3], is_i64, exactly(inst[1]), align_of(8)));
        BOOST_TEST(is_store_of(inst[4], is_i64, exactly(inst[3]), exactly(inst[2]), align_of(8)));
        BOOST_TEST(is_gep_of(inst[5], tuple_type, exactly(f->getArg(1)), constant(0), constant(1)));
        BOOST_TEST(is_gep_of(inst[6], tuple_type, exactly(inst[0]), constant(0), constant(1)));
        BOOST_TEST(is_mul_of(inst[7], exactly(inst[3]), constant(4)));
        BOOST_TEST(is_direct_call(inst[8], exactly(malloc_fn), call_arg(exactly(inst[7]))));
        BOOST_TEST(is_bitcast_of(inst[9], exactly(inst[8]), pointer_to(is_i8), pointer_to(is_i32)));
        BOOST_TEST(is_store_of(inst[10], pointer_to(is_i32), exactly(inst[9]), exactly(inst[6]), align_of(8)));
        BOOST_TEST(is_mul_of(inst[11], exactly(inst[3]), constant(4)));
        BOOST_TEST(is_bitcast_of(inst[12], exactly(inst[9]), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(inst[13], exactly(inst[5]), pointer_to(pointer_to(is_i32)), pointer_to(is_i8)));
        BOOST_TEST(
            is_direct_call(
                inst[14],
                is_memcpy,
                call_arg(exactly(inst[12]), attrs, align),
                call_arg(exactly(inst[13]), attrs, align),
                call_arg(exactly(inst[11])),
                call_arg(constant(false))));
        BOOST_TEST(is_return_of_void(inst[15]));
    }
}

BOOST_AUTO_TEST_SUITE_END()
