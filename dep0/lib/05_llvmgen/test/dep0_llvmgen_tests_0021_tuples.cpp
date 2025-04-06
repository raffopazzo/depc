/*
 * Copyright Raffaele Rossi 2023 - 2025.
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
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(struct_of())}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 1ul);
        BOOST_TEST(is_return_of_void(inst[0]));
    }
    {
        auto const f = get_function("f1");
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
        auto const f = get_function("f2");
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
        auto const f = get_function("f3");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 2ul);
        BOOST_TEST(is_direct_call(
            inst[0],
            exactly(get_function("f1")),
            call_arg(exactly(f->getArg(0ul))),
            call_arg(constant(0), {sext}),
            call_arg(constant(1), {zext})));
        BOOST_TEST(is_return_of_void(inst[1]));
    }
    {
        auto const f = get_function("f4");
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
            exactly(get_function("f1")),
            call_arg(exactly(gep_array_1)),
            call_arg(constant(2), {sext}),
            call_arg(constant(3), {zext})));
        BOOST_TEST(is_direct_call(
            call2,
            exactly(get_function("f3")),
            call_arg(exactly(gep_array_2))));

        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function("f5");
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
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(struct_of()), std::nullopt, nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = get_function("f1");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(1)), align_of(4))));
    }
    {
        auto const f = get_function("f2");
        auto const tuple_type = struct_of(is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0)), align_of(4))));
    }
    {
        auto const f = get_function("f3");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0)), align_of(4))));
    }
    {
        auto const f = get_function("f4");
        auto const tuple_type = struct_of(is_i32, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "xs", nonnull)}, is_i32, zext));
        auto const xs = exactly(f->getArg(0ul));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(get_function("f1")),
                    call_arg(gep_of(tuple_type, xs, constant(2))))));
    }
    {
        auto const f = get_function("f5");
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
        auto const f = get_function("f");
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
        auto const f = get_function("f2");
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
        auto const f = get_function("g");
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
        auto const f = get_function("f1");
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
        auto const f = get_function("f2");
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
    {
        auto const f = get_function("f1");
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
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 13ul);
        auto const gep_fst      = inst[0];
        auto const store_fst    = inst[1];
        auto const gep_snd      = inst[2];
        auto const malloccall   = inst[3];
        auto const i32_ptr      = inst[4];
        auto const gep_0        = inst[5];
        auto const store_0      = inst[6];
        auto const gep_1        = inst[7];
        auto const store_1      = inst[8];
        auto const gep_2        = inst[9];
        auto const store_2      = inst[10];
        auto const store_ptr    = inst[11];
        auto const ret          = inst[12];
        BOOST_TEST(is_gep_of(gep_fst, tuple_type, exactly(f->getArg(0ul)), constant(0), constant(0)));
        BOOST_TEST(is_store_of(store_fst, is_i64, constant(3), exactly(gep_fst), align_of(8)));
        BOOST_TEST(is_gep_of(gep_snd, tuple_type, exactly(f->getArg(0ul)), constant(0), constant(1)));
        BOOST_TEST(is_direct_call(malloccall, exactly(get_function("malloc")), call_arg(constant(12))));
        BOOST_TEST(is_bitcast_of(i32_ptr, exactly(malloccall), pointer_to(is_i8), pointer_to(is_i32)));
        BOOST_TEST(is_gep_of(gep_0, is_i32, exactly(i32_ptr), constant(0)));
        BOOST_TEST(is_store_of(store_0, is_i32, constant(1), exactly(gep_0), align_of(4)));
        BOOST_TEST(is_gep_of(gep_1, is_i32, exactly(i32_ptr), constant(1)));
        BOOST_TEST(is_store_of(store_1, is_i32, constant(2), exactly(gep_1), align_of(4)));
        BOOST_TEST(is_gep_of(gep_2, is_i32, exactly(i32_ptr), constant(2)));
        BOOST_TEST(is_store_of(store_2, is_i32, constant(3), exactly(gep_2), align_of(4)));
        BOOST_TEST(is_store_of(store_ptr, pointer_to(is_i32), exactly(i32_ptr), exactly(gep_snd), align_of(8)));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function("extract");
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
        auto const f = get_function("f3");
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
                exactly(get_function("extract")),
                call_arg(exactly(f->getArg(0))),
                call_arg(tmp)));
        BOOST_TEST(is_return_of_void(inst[inst.size() - 1]));
    }
    {
        auto const f = get_function("f4");
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
        BOOST_TEST(is_direct_call(call1, exactly(get_function("f2")), call_arg(exactly(gep1))));
        BOOST_TEST(is_direct_call(call2, exactly(get_function("f2")), call_arg(exactly(gep2))));
        BOOST_TEST(is_direct_call(call3, exactly(get_function("f2")), call_arg(exactly(gep3))));
    }
    {
        auto const f = get_function(".dtor.0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep      = inst[0];
        auto const load     = inst[1];
        auto const bitcast  = inst[2];
        auto const free     = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_gep_of(gep, tuple_type, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(bitcast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(bitcast))));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const alloca   = inst[0];
        auto const call_f2  = inst[1];
        auto const call_f1  = inst[2];
        auto const dtor     = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_alloca(alloca, tuple_type, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(call_f2, exactly(get_function("f2")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(call_f1, exactly(get_function("f1")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(dtor, exactly(get_function(".dtor.0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call_f1)));
    }
    {
        auto const f = get_function("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const inst = get_instructions(*entry);
        BOOST_TEST_REQUIRE(inst.size() == 6ul);
        auto const alloca   = inst[0];
        auto const call_f2  = inst[1];
        auto const call_f1  = inst[2];
        auto const cmp      = inst[3];
        auto const dtor     = inst[4];
        auto const br       = inst[5];
        BOOST_TEST(is_alloca(alloca, tuple_type, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(call_f2, exactly(get_function("f2")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(call_f1, exactly(get_function("f1")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(dtor, exactly(get_function(".dtor.0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_cmp(cmp, llvm::CmpInst::ICMP_EQ, exactly(call_f1), constant(0)));
        BOOST_TEST(is_branch_of(br, exactly(cmp), exactly(then0), exactly(else0)));
        BOOST_TEST(is_return_of(then0->getTerminator(), constant(1)));
        BOOST_TEST(is_return_of(else0->getTerminator(), constant(2)));
    }
    {
        auto const f = get_function("f7");
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
        BOOST_TEST_REQUIRE(inst.size() == 17ul);
        auto const gep_0        = inst[0];
        auto const gep_x_0      = inst[1];
        auto const gep_r_0      = inst[2];
        auto const load_x_0     = inst[3];
        auto const store_r_0    = inst[4];
        auto const gep_x_1      = inst[5];
        auto const gep_r_1      = inst[6];
        auto const malloc_size  = inst[7];
        auto const malloc_call  = inst[8];
        auto const bitcast_p32  = inst[9];
        auto const store_r_1    = inst[10];
        auto const load_x_1     = inst[11];
        auto const memcpy_size  = inst[12];
        auto const bitcast_r_1  = inst[13];
        auto const bitcast_x_1  = inst[14];
        auto const memcpy_call  = inst[15];
        auto const ret          = inst[16];
        BOOST_TEST(is_gep_of(gep_0, tuple_type, exactly(f->getArg(0)), constant(0)));
        BOOST_TEST(is_gep_of(gep_x_0, tuple_type, exactly(f->getArg(1)), constant(0), constant(0)));
        BOOST_TEST(is_gep_of(gep_r_0, tuple_type, exactly(gep_0), constant(0), constant(0)));
        BOOST_TEST(is_load_of(load_x_0, is_i64, exactly(gep_x_0), align_of(8)));
        BOOST_TEST(is_store_of(store_r_0, is_i64, exactly(load_x_0), exactly(gep_r_0), align_of(8)));
        BOOST_TEST(is_gep_of(gep_x_1, tuple_type, exactly(f->getArg(1)), constant(0), constant(1)));
        BOOST_TEST(is_gep_of(gep_r_1, tuple_type, exactly(gep_0), constant(0), constant(1)));
        BOOST_TEST(is_mul_of(malloc_size, exactly(load_x_0), constant(4)));
        BOOST_TEST(is_direct_call(malloc_call, exactly(get_function("malloc")), call_arg(exactly(malloc_size))));
        BOOST_TEST(is_bitcast_of(bitcast_p32, exactly(malloc_call), pointer_to(is_i8), pointer_to(is_i32)));
        BOOST_TEST(is_store_of(store_r_1, pointer_to(is_i32), exactly(bitcast_p32), exactly(gep_r_1), align_of(8)));
        BOOST_TEST(is_load_of(load_x_1, pointer_to(is_i32), exactly(gep_x_1), align_of(8)));
        BOOST_TEST(is_mul_of(memcpy_size, exactly(load_x_0), constant(4)));
        BOOST_TEST(is_bitcast_of(bitcast_r_1, exactly(bitcast_p32), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(bitcast_x_1, exactly(load_x_1), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(
            is_direct_call(
                memcpy_call,
                is_memcpy,
                call_arg(exactly(bitcast_r_1), attrs, align),
                call_arg(exactly(bitcast_x_1), attrs, align),
                call_arg(exactly(memcpy_size)),
                call_arg(constant(false))));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(tuple_type)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const alloca   = inst[0];
        auto const call_f2  = inst[1];
        auto const call_f7  = inst[2];
        auto const dtor     = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_alloca(alloca, tuple_type, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(call_f2, exactly(get_function("f2")), call_arg(exactly(alloca))));
        BOOST_TEST(
            is_direct_call(
                call_f7,
                exactly(get_function("f7")),
                call_arg(exactly(f->getArg(0))),
                call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(dtor, exactly(get_function(".dtor.0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function(".dtor.1");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(tuple_type), {}, nonnull),
                    arg_of(is_i64, {}, zext)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry    = blks[0];
        auto const loop     = blks[1];
        auto const next     = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_cmp(inst[0], llvm::CmpInst::ICMP_UGT, exactly(f->getArg(1)), constant(0)));
            BOOST_TEST(is_branch_of(inst[1], exactly(inst[0]), exactly(loop), exactly(next)));
        }
        {
            auto const inst = get_instructions(*loop);
            BOOST_TEST_REQUIRE(inst.size() == 6ul);
            BOOST_TEST(
                is_phi_of(
                    inst[0], is_i64,
                    std::pair{exactly(f->getArg(1)), entry},
                    std::pair{exactly(inst[1]), loop}));
            BOOST_TEST(is_sub_of(inst[1], exactly(inst[0]), constant(1)));
            BOOST_TEST(is_gep_of(inst[2], tuple_type, exactly(f->getArg(0)), exactly(inst[1])));
            BOOST_TEST(is_direct_call(inst[3], exactly(get_function(".dtor.0")), call_arg(exactly(inst[2]))));
            BOOST_TEST(is_cmp(inst[4], llvm::CmpInst::ICMP_UGT, exactly(inst[1]), constant(0)));
            BOOST_TEST(is_branch_of(inst[5], exactly(inst[4]), exactly(loop), exactly(next)));
        }
        {
            auto const inst = get_instructions(*next);
            BOOST_TEST_REQUIRE(inst.size() == 1ul);
            BOOST_TEST(is_return_of_void(inst[0]));
        }
    }
    {
        auto const f = get_function("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 6ul);
        auto const alloca   = inst[0];
        auto const call_f8  = inst[1];
        auto const gep      = inst[2];
        auto const call_f1  = inst[3];
        auto const dtor     = inst[4];
        auto const ret      = inst[5];
        BOOST_TEST(is_alloca(alloca, tuple_type, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(call_f8, exactly(get_function("f8")), call_arg(exactly(alloca))));
        BOOST_TEST(is_gep_of(gep, tuple_type, exactly(alloca), constant(0)));
        BOOST_TEST(is_direct_call(call_f1, exactly(get_function("f1")), call_arg(exactly(gep))));
        BOOST_TEST(
            is_direct_call(
                dtor,
                exactly(get_function(".dtor.1")),
                call_arg(exactly(alloca)),
                call_arg(constant(1), zext)));
        BOOST_TEST(is_return_of(ret, exactly(call_f1)));
    }
    {
        auto const f = get_function("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "which", zext)}, is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 7ul);
        auto const entry    = blks[0];
        auto const then0    = blks[1];
        auto const else0    = blks[2];
        auto const cont     = blks[3];
        auto const then1    = blks[4];
        auto const else2    = blks[5];
        auto const cont3    = blks[6];
        llvm::Value const* size;
        llvm::Value const* size2;
        llvm::Value const* f2_result;
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 4ul);
            size            = inst[0];
            f2_result       = inst[1];
            size2           = inst[2];
            auto const br   = inst[3];
            BOOST_TEST(is_alloca(size, is_i64, constant(1), align_of(8)));
            BOOST_TEST(is_alloca(f2_result, tuple_type, constant(1), align_of(8)));
            BOOST_TEST(is_alloca(size2, is_i64, constant(1), align_of(8)));
            BOOST_TEST(is_branch_of(br, exactly(f->getArg(0)), exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], is_i64, constant(1), exactly(size), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont)));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], is_i64, constant(2), exactly(size), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont)));
        }
        llvm::Value const* copy_result;
        llvm::Value const* call_f1;
        {
            auto const inst = get_instructions(*cont);
            BOOST_TEST_REQUIRE(inst.size() == 7ul);
            auto const load     = inst[0];
            copy_result         = inst[1];
            auto const call_f2  = inst[2];
            auto const copy     = inst[3];
            auto const gep      = inst[4];
            call_f1             = inst[5];
            auto const br       = inst[6];
            BOOST_TEST(is_load_of(load, is_i64, exactly(size), align_of(8)));
            BOOST_TEST(is_alloca(copy_result, tuple_type, exactly(load), align_of(8)));
            BOOST_TEST(is_direct_call(call_f2, exactly(get_function("f2")), call_arg(exactly(f2_result))));
            BOOST_TEST(
                is_direct_call(
                    copy,
                    exactly(get_function("copy")),
                    call_arg(exactly(copy_result)),
                    call_arg(exactly(f->getArg(0)), zext),
                    call_arg(exactly(f2_result))));
            BOOST_TEST(is_gep_of(gep, tuple_type, exactly(copy_result), constant(0)));
            BOOST_TEST(is_direct_call(call_f1, exactly(get_function("f1")), call_arg(exactly(gep))));
            BOOST_TEST(is_branch_of(br, exactly(f->getArg(0)), exactly(then1), exactly(else2)));
        }
        {
            auto const inst = get_instructions(*then1);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], is_i64, constant(1), exactly(size2), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont3)));
        }
        {
            auto const inst = get_instructions(*else2);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], is_i64, constant(2), exactly(size2), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont3)));
        }
        {
            auto const inst = get_instructions(*cont3);
            BOOST_TEST_REQUIRE(inst.size() == 4ul);
            auto const load     = inst[0];
            auto const dtor_1   = inst[1];
            auto const dtor_0   = inst[2];
            auto const ret      = inst[3];
            BOOST_TEST(is_load_of(load, is_i64, exactly(size2), align_of(8)));
            BOOST_TEST(
                is_direct_call(
                    dtor_1,
                    exactly(get_function(".dtor.1")),
                    call_arg(exactly(copy_result)),
                    call_arg(exactly(load), zext)));
            BOOST_TEST(
                is_direct_call(
                    dtor_0,
                    exactly(get_function(".dtor.0")),
                    call_arg(exactly(f2_result))));
            BOOST_TEST(is_return_of(ret, exactly(call_f1)));
        }
    }
}

BOOST_AUTO_TEST_CASE(pass_004_normalized)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_004.depc"));
    {
        auto const f = get_function("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 1ul);
        BOOST_TEST(is_return_of(inst[0], constant(3)));
    }
    {
        auto const f = get_function("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 1ul);
        BOOST_TEST(is_return_of(inst[0], constant(2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_005.depc"));
    auto const tuple_type = struct_of(is_i64, pointer_to(is_i32));
    auto const arg_type = struct_of(is_i64, pointer_to(tuple_type));
    {
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const alloca   = inst[0];
        auto const call1    = inst[1];
        auto const call2    = inst[2];
        auto const dtor0    = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_alloca(alloca, arg_type, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(call1, exactly(get_function("make_vector_of_vectors")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(call2, exactly(get_function("use_vector_of_vectors")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(dtor0, exactly(get_function(".dtor.0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call2)));
    }
    {
        auto const f = get_function(".dtor.0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(arg_type), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 8ul);
        auto const gep0     = inst[0];
        auto const load0    = inst[1];
        auto const gep1     = inst[2];
        auto const load1    = inst[3];
        auto const dtor1    = inst[4];
        auto const bitcast  = inst[5];
        auto const free     = inst[6];
        auto const ret      = inst[7];
        BOOST_TEST(is_gep_of(gep0, arg_type, exactly(f->getArg(0)), constant(0), constant(0)));
        BOOST_TEST(is_load_of(load0, is_i64, exactly(gep0), align_of(8)));
        BOOST_TEST(is_gep_of(gep1, arg_type, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load1, pointer_to(tuple_type), exactly(gep1), align_of(8)));
        BOOST_TEST(
            is_direct_call(
                dtor1,
                exactly(get_function(".dtor.1")),
                call_arg(exactly(load1)),
                call_arg(exactly(load0), zext)));
        BOOST_TEST(is_bitcast_of(bitcast, exactly(load1), pointer_to(tuple_type), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(bitcast))));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function(".dtor.1");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(tuple_type), {}, nonnull),
                    arg_of(is_i64, {}, zext)
                },
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry    = blks[0];
        auto const loop     = blks[1];
        auto const next     = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_cmp(inst[0], llvm::CmpInst::Predicate::ICMP_UGT, exactly(f->getArg(1)), constant(0)));
            BOOST_TEST(is_branch_of(inst[1], exactly(inst[0]), exactly(loop), exactly(next)));
        }
        {
            auto const inst = get_instructions(*loop);
            BOOST_TEST_REQUIRE(inst.size() == 6ul);
            auto const phi      = inst[0];
            auto const sub      = inst[1];
            auto const gep      = inst[2];
            auto const dtor2    = inst[3];
            auto const cmp      = inst[4];
            auto const br       = inst[5];
            BOOST_TEST(is_phi_of(phi, is_i64, std::pair{exactly(f->getArg(1)), entry}, std::pair{exactly(sub), loop}));
            BOOST_TEST(is_sub_of(sub, exactly(phi), constant(1)));
            BOOST_TEST(is_gep_of(gep, tuple_type, exactly(f->getArg(0)), exactly(sub)));
            BOOST_TEST(is_direct_call(dtor2, exactly(get_function(".dtor.2")), call_arg(exactly(gep))));
            BOOST_TEST(is_cmp(cmp, llvm::CmpInst::Predicate::ICMP_UGT, exactly(sub), constant(0)));
            BOOST_TEST(is_branch_of(br, exactly(cmp), exactly(loop), exactly(next)));
        }
        {
            auto const inst = get_instructions(*next);
            BOOST_TEST_REQUIRE(inst.size() == 1ul);
            BOOST_TEST(is_return_of_void(inst[0]));
        }
    }
    {
        auto const f = get_function(".dtor.2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep      = inst[0];
        auto const load     = inst[1];
        auto const bitcast  = inst[2];
        auto const free     = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_gep_of(gep, tuple_type, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(bitcast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(bitcast))));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_006.depc"));
    auto const tuple_type = struct_of(is_i64, pointer_to(is_i32));
    {
        auto const f = get_function("copy_0");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{
                    ret_ptr_to(tuple_type),
                    arg_of(pointer_to(tuple_type), "x", nonnull)},
                is_void));
        BOOST_TEST(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
    {
        auto const f = get_function("copy_1");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{
                    ret_ptr_to(tuple_type),
                    arg_of(pointer_to(tuple_type), "x", nonnull)},
                is_void));
        BOOST_TEST(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 18ul);
        auto const gep_x_0      = inst[0];
        auto const gep_r_0      = inst[1];
        auto const gep_x_0_0    = inst[2];
        auto const gep_r_0_0    = inst[3];
        auto const load_x_0_0   = inst[4];
        auto const store_r_0_0  = inst[5];
        auto const gep_x_0_1    = inst[6];
        auto const gep_r_0_1    = inst[7];
        auto const malloc_size  = inst[8];
        auto const malloc_call  = inst[9];
        auto const bitcast_1    = inst[10];
        auto const store_ptr    = inst[11];
        auto const load_ptr     = inst[12];
        auto const memcpy_size  = inst[13];
        auto const bitcast_2    = inst[14];
        auto const bitcast_3    = inst[15];
        auto const memcpy_call  = inst[16];
        auto const ret          = inst[17];
        BOOST_TEST(is_gep_of(gep_x_0, tuple_type, exactly(f->getArg(1)), constant(0)));
        BOOST_TEST(is_gep_of(gep_r_0, tuple_type, exactly(f->getArg(0)), constant(0)));
        BOOST_TEST(is_gep_of(gep_x_0_0, tuple_type, exactly(gep_x_0), constant(0), constant(0)));
        BOOST_TEST(is_gep_of(gep_r_0_0, tuple_type, exactly(gep_r_0), constant(0), constant(0)));
        BOOST_TEST(is_load_of(load_x_0_0, is_i64, exactly(gep_x_0_0), align_of(8)));
        BOOST_TEST(is_store_of(store_r_0_0, is_i64, exactly(load_x_0_0), exactly(gep_r_0_0), align_of(8)));
        BOOST_TEST(is_gep_of(gep_x_0_1, tuple_type, exactly(gep_x_0), constant(0), constant(1)));
        BOOST_TEST(is_gep_of(gep_r_0_1, tuple_type, exactly(gep_r_0), constant(0), constant(1)));
        BOOST_TEST(is_mul_of(malloc_size, exactly(load_x_0_0), constant(4)));
        BOOST_TEST(is_direct_call(malloc_call, exactly(get_function("malloc")), call_arg(exactly(malloc_size))));
        BOOST_TEST(is_bitcast_of(bitcast_1, exactly(malloc_call), pointer_to(is_i8), pointer_to(is_i32)));
        BOOST_TEST(is_store_of(store_ptr, pointer_to(is_i32), exactly(bitcast_1), exactly(gep_r_0_1), align_of(8)));
        BOOST_TEST(is_load_of(load_ptr, pointer_to(is_i32), exactly(gep_x_0_1), align_of(8)));
        BOOST_TEST(is_mul_of(memcpy_size, exactly(load_x_0_0), constant(4)));
        BOOST_TEST(is_bitcast_of(bitcast_2, exactly(bitcast_1), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(bitcast_3, exactly(load_ptr), pointer_to(is_i32), pointer_to(is_i8)));
        auto const attrs = std::vector{llvm::Attribute::Alignment};
        auto const align = llvm::Align(4);
        BOOST_TEST(
            is_direct_call(
                memcpy_call,
                is_memcpy,
                call_arg(exactly(bitcast_2), attrs, align),
                call_arg(exactly(bitcast_3), attrs, align),
                call_arg(exactly(memcpy_size)),
                call_arg(constant(false))));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function("copy_k");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{
                    ret_ptr_to(tuple_type),
                    arg_of(is_i64, "k", zext),
                    arg_of(pointer_to(tuple_type), "x", nonnull)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry    = blks[0];
        auto const loop     = blks[1];
        auto const next     = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_cmp(inst[0], llvm::CmpInst::Predicate::ICMP_ULT, constant(0), exactly(f->getArg(1))));
            BOOST_TEST(is_branch_of(inst[1], exactly(inst[0]), exactly(loop), exactly(next)));
        }
        {
            auto const inst = get_instructions(*loop);
            BOOST_TEST_REQUIRE(inst.size() == 21ul);
            auto const phi          = inst[0];
            auto const inc          = inst[1];
            auto const gep_x_i      = inst[2];
            auto const gep_r_i      = inst[3];
            auto const gep_x_i_0    = inst[4];
            auto const gep_r_i_0    = inst[5];
            auto const load_x_i_0   = inst[6];
            auto const store_r_i_0  = inst[7];
            auto const gep_x_i_1    = inst[8];
            auto const gep_r_i_1    = inst[9];
            auto const malloc_size  = inst[10];
            auto const malloc_call  = inst[11];
            auto const bitcast_1    = inst[12];
            auto const store_ptr    = inst[13];
            auto const load_ptr     = inst[14];
            auto const memcpy_size  = inst[15];
            auto const bitcast_2    = inst[16];
            auto const bitcast_3    = inst[17];
            auto const memcpy_call  = inst[18];
            auto const cmp          = inst[19];
            auto const br           = inst[20];
            BOOST_TEST(is_phi_of(phi, is_i64, std::pair{constant(0), entry}, std::pair{exactly(inc), loop}));
            BOOST_TEST(is_add_of(inc, exactly(phi), constant(1)));
            BOOST_TEST(is_gep_of(gep_x_i, tuple_type, exactly(f->getArg(2)), exactly(phi)));
            BOOST_TEST(is_gep_of(gep_r_i, tuple_type, exactly(f->getArg(0)), exactly(phi)));
            BOOST_TEST(is_gep_of(gep_x_i_0, tuple_type, exactly(gep_x_i), constant(0), constant(0)));
            BOOST_TEST(is_gep_of(gep_r_i_0, tuple_type, exactly(gep_r_i), constant(0), constant(0)));
            BOOST_TEST(is_load_of(load_x_i_0, is_i64, exactly(gep_x_i_0), align_of(8)));
            BOOST_TEST(is_store_of(store_r_i_0, is_i64, exactly(load_x_i_0), exactly(gep_r_i_0), align_of(8)));
            BOOST_TEST(is_gep_of(gep_x_i_1, tuple_type, exactly(gep_x_i), constant(0), constant(1)));
            BOOST_TEST(is_gep_of(gep_r_i_1, tuple_type, exactly(gep_r_i), constant(0), constant(1)));
            BOOST_TEST(is_mul_of(malloc_size, exactly(load_x_i_0), constant(4)));
            BOOST_TEST(is_direct_call(malloc_call, exactly(get_function("malloc")), call_arg(exactly(malloc_size))));
            BOOST_TEST(is_bitcast_of(bitcast_1, exactly(malloc_call), pointer_to(is_i8), pointer_to(is_i32)));
            BOOST_TEST(is_store_of(store_ptr, pointer_to(is_i32), exactly(bitcast_1), exactly(gep_r_i_1), align_of(8)));
            BOOST_TEST(is_load_of(load_ptr, pointer_to(is_i32), exactly(gep_x_i_1), align_of(8)));
            BOOST_TEST(is_mul_of(memcpy_size, exactly(load_x_i_0), constant(4)));
            BOOST_TEST(is_bitcast_of(bitcast_2, exactly(bitcast_1), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(bitcast_3, exactly(load_ptr), pointer_to(is_i32), pointer_to(is_i8)));
            auto const attrs = std::vector{llvm::Attribute::Alignment};
            auto const align = llvm::Align(4);
            BOOST_TEST(
                is_direct_call(
                    memcpy_call,
                    is_memcpy,
                    call_arg(exactly(bitcast_2), attrs, align),
                    call_arg(exactly(bitcast_3), attrs, align),
                    call_arg(exactly(memcpy_size)),
                    call_arg(constant(false))));
            BOOST_TEST(is_cmp(cmp, llvm::CmpInst::Predicate::ICMP_ULT, exactly(inc), exactly(f->getArg(1))));
            BOOST_TEST(is_branch_of(br, exactly(cmp), exactly(loop), exactly(next)));
        }
        {
            auto const inst = get_instructions(*next);
            BOOST_TEST_REQUIRE(inst.size() == 1ul);
            BOOST_TEST(is_return_of_void(inst[0]));
        }
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_007.depc"));
    auto const tuple_type_1 = struct_of(is_i16, pointer_to(is_i32));
    auto const tuple_type_2 = struct_of(is_i64, tuple_type_1);
    {
        auto const f = get_function("test_it");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const alloca   = inst[0];
        auto const make_it  = inst[1];
        auto const use_it   = inst[2];
        auto const dtor     = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_alloca(alloca, tuple_type_2, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(make_it, exactly(get_function("make_it")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(use_it, exactly(get_function("use_it")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(dtor, exactly(get_function(".dtor.0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(use_it)));
    }
    {
        auto const f = get_function(".dtor.0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type_2), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 3ul);
        BOOST_TEST(is_gep_of(inst[0], tuple_type_2, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_direct_call(inst[1], exactly(get_function(".dtor.1")), call_arg(exactly(inst[0]))));
        BOOST_TEST(is_return_of_void(inst[2]));
    }
    {
        auto const f = get_function(".dtor.1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type_1), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep      = inst[0];
        auto const load     = inst[1];
        auto const bitcast  = inst[2];
        auto const free     = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_gep_of(gep, tuple_type_1, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(bitcast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(bitcast))));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    BOOST_TEST_REQUIRE(pass("0021_tuples/pass_008.depc"));
    auto const tuple_type = struct_of(is_i64, pointer_to(is_i32));
    {
        auto const f = get_function("test_it");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const alloca   = inst[0];
        auto const make_it  = inst[1];
        auto const use_it   = inst[2];
        auto const dtor     = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_alloca(alloca, tuple_type, constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(make_it, exactly(get_function("make_it")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(use_it, exactly(get_function("use_it")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(dtor, exactly(get_function(".dtor.0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(use_it)));
    }
    {
        auto const f = get_function(".dtor.0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep      = inst[0];
        auto const load     = inst[1];
        auto const bitcast  = inst[2];
        auto const free     = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_gep_of(gep, tuple_type, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(bitcast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(bitcast))));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_SUITE_END()
