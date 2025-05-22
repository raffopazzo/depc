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
        BOOST_TEST(is_block_of(*else0, std::tuple{return_of(constant(0))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0022_structs/pass_002.depc"));
    auto const t = get_struct("t");
    BOOST_TEST(is_struct(t, "t", is_i64, pointer_to(is_i32)));
    {
        auto const f = get_function("make_array");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_ptr_to(exactly(t))}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 10ul);
        auto const gep0     = inst[0];
        auto const gep0_0   = inst[1];
        auto const store_1  = inst[2];
        auto const gep0_1   = inst[3];
        auto const malloc   = inst[4];
        auto const bitcast  = inst[5];
        auto const gep_p    = inst[6];
        auto const store_2  = inst[7];
        auto const store_p  = inst[8];
        auto const ret      = inst[9];
        BOOST_TEST(is_gep_of(gep0, exactly(t), exactly(f->getArg(0)), constant(0)));
        BOOST_TEST(is_gep_of(gep0_0, exactly(t), exactly(gep0), constant(0), constant(0)));
        BOOST_TEST(is_store_of(store_1, is_i64, constant(1), exactly(gep0_0), align_of(8)));
        BOOST_TEST(is_gep_of(gep0_1, exactly(t), exactly(gep0), constant(0), constant(1)));
        BOOST_TEST(is_direct_call(malloc, exactly(get_function("malloc")), call_arg(constant(4))));
        BOOST_TEST(is_bitcast_of(bitcast, exactly(malloc), pointer_to(is_i8), pointer_to(is_i32)));
        BOOST_TEST(is_gep_of(gep_p, is_i32, exactly(bitcast), constant(0)));
        BOOST_TEST(is_store_of(store_2, is_i32, constant(2), exactly(gep_p), align_of(4)));
        BOOST_TEST(is_store_of(store_p, pointer_to(is_i32), exactly(bitcast), exactly(gep0_1), align_of(8)));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function("use_array");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), "x", nonnull)}, is_i1, zext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep0     = inst[0];
        auto const gep0_0   = inst[1];
        auto const load     = inst[2];
        auto const cmp      = inst[3];
        auto const ret      = inst[4];
        BOOST_TEST(is_gep_of(gep0, exactly(t), exactly(f->getArg(0)), constant(0)));
        BOOST_TEST(is_gep_of(gep0_0, exactly(t), exactly(gep0), constant(0), constant(0)));
        BOOST_TEST(is_load_of(load, is_i64, exactly(gep0_0), align_of(8)));
        BOOST_TEST(is_cmp(cmp, llvm::CmpInst::Predicate::ICMP_UGT, exactly(load), constant(0)));
        BOOST_TEST(is_return_of(ret, exactly(cmp)));
    }
    {
        auto const f = get_function("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 5ul);
            auto const alloca   = inst[0];
            auto const call1    = inst[1];
            auto const call2    = inst[2];
            auto const dtor     = inst[3];
            auto const br       = inst[4];
            BOOST_TEST(is_alloca(alloca, exactly(t), constant(1), align_of(8)));
            BOOST_TEST(is_direct_call(call1, exactly(get_function("make_array")), call_arg(exactly(alloca))));
            BOOST_TEST(is_direct_call(call2, exactly(get_function("use_array")), call_arg(exactly(alloca))));
            BOOST_TEST(
                is_direct_call(
                    dtor,
                    exactly(get_function(".dtor.0")),
                    call_arg(exactly(alloca)),
                    call_arg(constant(1), zext)));
            BOOST_TEST(is_branch_of(br, exactly(call2), exactly(then0), exactly(else0)));
        }
        BOOST_TEST(is_block_of(*then0, std::tuple{return_of(constant(1))}));
        BOOST_TEST(is_block_of(*else0, std::tuple{return_of(constant(0))}));
    }
    {
        auto const f = get_function(".dtor.0");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(exactly(t)), {}, nonnull),
                    arg_of(is_i64, {}, zext)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const loop  = blks[1];
        auto const next  = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_cmp(inst[0], llvm::CmpInst::Predicate::ICMP_UGT, exactly(f->getArg(1)), constant(0)));
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
            BOOST_TEST(is_gep_of(inst[2], exactly(t), exactly(f->getArg(0)), exactly(inst[1])));
            BOOST_TEST(is_direct_call(inst[3], exactly(get_function(".dtor.1")), call_arg(exactly(inst[2]))));
            BOOST_TEST(is_cmp(inst[4], llvm::CmpInst::ICMP_UGT, exactly(inst[1]), constant(0)));
            BOOST_TEST(is_branch_of(inst[5], exactly(inst[4]), exactly(loop), exactly(next)));
        }
        BOOST_TEST(is_block_of(*next, std::tuple{is_return_of_void}));
    }
    {
        auto const f = get_function(".dtor.1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep  = inst[0];
        auto const load = inst[1];
        auto const cast = inst[2];
        auto const free = inst[3];
        auto const ret  = inst[4];
        BOOST_TEST(is_gep_of(gep, exactly(t), exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(cast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(cast))));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0022_structs/pass_003.depc"));
    auto const t = get_struct("t");
    auto const tuple_type = struct_of(is_i64, pointer_to(is_i32));
    BOOST_TEST(is_struct(t, "t", is_i64, pointer_to(tuple_type)));
    {
        auto const f = get_function(".dtor.0");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(exactly(t)), {}, nonnull),
                    arg_of(is_i64, {}, zext)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const loop  = blks[1];
        auto const next  = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_cmp(inst[0], llvm::CmpInst::Predicate::ICMP_UGT, exactly(f->getArg(1)), constant(0)));
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
            BOOST_TEST(is_gep_of(inst[2], exactly(t), exactly(f->getArg(0)), exactly(inst[1])));
            BOOST_TEST(is_direct_call(inst[3], exactly(get_function(".dtor.1")), call_arg(exactly(inst[2]))));
            BOOST_TEST(is_cmp(inst[4], llvm::CmpInst::ICMP_UGT, exactly(inst[1]), constant(0)));
            BOOST_TEST(is_branch_of(inst[5], exactly(inst[4]), exactly(loop), exactly(next)));
        }
        BOOST_TEST(is_block_of(*next, std::tuple{is_return_of_void}));
    }
    {
        auto const f = get_function(".dtor.1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 8ul);
        auto const gep0  = inst[0];
        auto const load0 = inst[1];
        auto const gep1  = inst[2];
        auto const load1 = inst[3];
        auto const dtor  = inst[4];
        auto const cast  = inst[5];
        auto const free  = inst[6];
        auto const ret   = inst[7];
        BOOST_TEST(is_gep_of(gep0, exactly(t), exactly(f->getArg(0)), constant(0), constant(0)));
        BOOST_TEST(is_load_of(load0, is_i64, exactly(gep0), align_of(8)));
        BOOST_TEST(is_gep_of(gep1, exactly(t), exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load1, pointer_to(tuple_type), exactly(gep1), align_of(8)));
        BOOST_TEST(
            is_direct_call(
                dtor,
                exactly(get_function(".dtor.2")),
                call_arg(exactly(load1)),
                call_arg(exactly(load0), zext)));
        BOOST_TEST(is_bitcast_of(cast, exactly(load1), pointer_to(tuple_type), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(cast))));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function(".dtor.2");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(tuple_type), {}, nonnull),
                    arg_of(is_i64, {}, zext)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const loop  = blks[1];
        auto const next  = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_cmp(inst[0], llvm::CmpInst::Predicate::ICMP_UGT, exactly(f->getArg(1)), constant(0)));
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
            BOOST_TEST(is_direct_call(inst[3], exactly(get_function(".dtor.3")), call_arg(exactly(inst[2]))));
            BOOST_TEST(is_cmp(inst[4], llvm::CmpInst::ICMP_UGT, exactly(inst[1]), constant(0)));
            BOOST_TEST(is_branch_of(inst[5], exactly(inst[4]), exactly(loop), exactly(next)));
        }
        BOOST_TEST(is_block_of(*next, std::tuple{is_return_of_void}));
    }
    {
        auto const f = get_function(".dtor.3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep  = inst[0];
        auto const load = inst[1];
        auto const cast = inst[2];
        auto const free = inst[3];
        auto const ret  = inst[4];
        BOOST_TEST(is_gep_of(gep, tuple_type, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(cast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(cast))));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0022_structs/pass_004.depc"));
    auto const t = get_struct("t");
    auto const tuple_type = struct_of(is_i64, pointer_to(is_i32));
    BOOST_TEST(is_struct(t, "t", tuple_type));
    {
        auto const f = get_function(".dtor.0");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(exactly(t)), {}, nonnull),
                    arg_of(is_i64, {}, zext)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const loop  = blks[1];
        auto const next  = blks[2];
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_cmp(inst[0], llvm::CmpInst::Predicate::ICMP_UGT, exactly(f->getArg(1)), constant(0)));
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
            BOOST_TEST(is_gep_of(inst[2], exactly(t), exactly(f->getArg(0)), exactly(inst[1])));
            BOOST_TEST(is_direct_call(inst[3], exactly(get_function(".dtor.1")), call_arg(exactly(inst[2]))));
            BOOST_TEST(is_cmp(inst[4], llvm::CmpInst::ICMP_UGT, exactly(inst[1]), constant(0)));
            BOOST_TEST(is_branch_of(inst[5], exactly(inst[4]), exactly(loop), exactly(next)));
        }
        BOOST_TEST(is_block_of(*next, std::tuple{is_return_of_void}));
    }
    {
        auto const f = get_function(".dtor.1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 3ul);
        BOOST_TEST(is_gep_of(inst[0], exactly(t), exactly(f->getArg(0)), constant(0), constant(0)));
        BOOST_TEST(is_direct_call(inst[1], exactly(get_function(".dtor.2")), call_arg(exactly(inst[0]))));
        BOOST_TEST(is_return_of_void(inst[2]));
    }
    {
        auto const f = get_function(".dtor.2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), {}, nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep  = inst[0];
        auto const load = inst[1];
        auto const cast = inst[2];
        auto const free = inst[3];
        auto const ret  = inst[4];
        BOOST_TEST(is_gep_of(gep, tuple_type, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(gep), align_of(8)));
        BOOST_TEST(is_bitcast_of(cast, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_direct_call(free, exactly(get_function("free")), call_arg(exactly(cast))));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_SUITE_END()
