/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_llvmgen_tests_0023_references
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0023_references, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_000.depc"));
    auto const t = get_struct("t");
    BOOST_TEST(is_struct(t, "t", is_i32));
    {
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "x", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, exactly(f->getArg(0)), align_of(4))));
    }
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "x", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, exactly(f->getArg(0)), align_of(4))));
    }
    {
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(pointer_to(is_i32)), "x", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(
                    is_i32,
                    load_of(
                        pointer_to(is_i32),
                        gep_of(pointer_to(is_i32), exactly(f->getArg(0)), constant(0)),
                        align_of(8)),
                    align_of(4))));
    }
    {
        auto const f = get_function("f3");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), "p", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(
                    is_i32,
                    gep_of(exactly(t), exactly(f->getArg(0)), constant(0), constant(0)),
                    align_of(4))));
    }
    {
        auto const f = get_function("f4");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(pointer_to(exactly(t))), "x", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(
                    is_i32,
                    gep_of(
                        exactly(t),
                        load_of(
                            pointer_to(exactly(t)),
                            gep_of(pointer_to(exactly(t)), exactly(f->getArg(0)), constant(0)),
                            align_of(8)),
                        constant(0),
                        constant(0)),
                    align_of(4))));
    }
    {
        auto const f = get_function("f5");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{
                    ret_ptr_to(is_i32),
                    arg_of(pointer_to(is_i32), "p", nonnull)},
                is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 4ul);
        auto const gep   = inst[0ul];
        auto const load  = inst[1ul];
        auto const store = inst[2ul];
        auto const ret   = inst[3ul];
        BOOST_TEST(is_gep_of(gep, is_i32, exactly(f->getArg(0)), constant(0)));
        BOOST_TEST(is_load_of(load, is_i32, exactly(f->getArg(1)), align_of(4)));
        BOOST_TEST(is_store_of(store, is_i32, exactly(load), exactly(gep), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = get_function("f6");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{
                    ret_ptr_to(exactly(t)),
                    arg_of(pointer_to(exactly(t)), "p", nonnull)},
                is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep   = inst[0ul];
        auto const dst   = inst[1ul];
        auto const src   = inst[2ul];
        auto const cpy   = inst[3ul];
        auto const ret   = inst[4ul];
        BOOST_TEST(is_gep_of(gep, exactly(t), exactly(f->getArg(0)), constant(0)));
        BOOST_TEST(is_bitcast_of(dst, exactly(gep), pointer_to(exactly(t)), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(src, exactly(f->getArg(1)), pointer_to(exactly(t)), pointer_to(is_i8)));
        auto const attrs = std::vector{llvm::Attribute::Alignment};
        auto const align = llvm::Align(8);
        BOOST_TEST(
            is_direct_call(
                cpy,
                is_memcpy,
                call_arg(exactly(dst), attrs, align),
                call_arg(exactly(src), attrs, align),
                call_arg(constant(4)),
                call_arg(constant(false))));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_001.depc"));
    BOOST_TEST(get_function("puts"));
    BOOST_TEST(get_function("f0"));
    auto const f0 = get_function("f0");
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), direct_call_of(exactly(f0))));
    }
    for (auto const name: {"f2", "f3", "f4", "f5"})
    {
        BOOST_TEST_CONTEXT(name);
        auto const f = get_function(name);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), direct_call_of(exactly(f0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_002.depc"));
    {
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "x", nonnull)}, pointer_to(is_i32)));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(exactly(f->getArg(0)))}));
    }
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i1, "which", zext),
                    arg_of(pointer_to(is_i32), "x", nonnull),
                    arg_of(pointer_to(is_i32), "y", nonnull)},
                pointer_to(is_i32)));
        auto const b = get_blocks(*f);
        BOOST_TEST_REQUIRE(b.size() == 3ul);
        BOOST_TEST(is_block_of(*b[0], std::tuple{branch_of(exactly(f->getArg(0)), exactly(b[1]), exactly(b[2]))}));
        BOOST_TEST(is_block_of(*b[1], std::tuple{return_of(exactly(f->getArg(1)))}));
        BOOST_TEST(is_block_of(*b[2], std::tuple{return_of(exactly(f->getArg(2)))}));
    }
    {
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "a", sext)}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size()  == 8ul);
        auto const alloca   = inst[0];
        auto const store    = inst[1];
        auto const call1    = inst[2];
        auto const load1    = inst[3];
        auto const call2    = inst[4];
        auto const load2    = inst[5];
        auto const add      = inst[6];
        auto const ret      = inst[7];
        BOOST_TEST(is_alloca(alloca, is_i32, constant(1), align_of(4)));
        BOOST_TEST(is_store_of(store, is_i32, exactly(f->getArg(0)), exactly(alloca), align_of(4)));
        BOOST_TEST(is_direct_call(call1, exactly(get_function("f0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_load_of(load1, is_i32, exactly(call1), align_of(4)));
        BOOST_TEST(is_direct_call(call2, exactly(get_function("f0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_load_of(load2, is_i32, exactly(call2), align_of(4)));
        BOOST_TEST(is_add_of(add, exactly(load1), exactly(load2)));
        BOOST_TEST(is_return_of(ret, exactly(add)));
    }
    auto const t = get_struct("t");
    BOOST_TEST(is_struct(t, "t", is_i32));
    {
        auto const f = get_function("f3");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), "x", nonnull)}, pointer_to(exactly(t))));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0))));
    }
    {
        auto const f = get_function("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), "a", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(
                    is_i32,
                    gep_of(
                        exactly(t),
                        direct_call_of(exactly(get_function("f3")), call_arg(exactly(f->getArg(0)))),
                        constant(0),
                        constant(0)),
                    align_of(4))));
    }
    {
        auto const f = get_function("f5");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i1, "which", zext),
                    arg_of(is_i32, "a", sext),
                    arg_of(is_i32, "b", sext)
                },
                is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST(inst.size() == 10ul);
        auto const alloca0 = inst[0];
        auto const alloca1 = inst[1];
        auto const store0  = inst[2];
        auto const store1  = inst[3];
        auto const call0   = inst[4];
        auto const load0   = inst[5];
        auto const call1   = inst[6];
        auto const load1   = inst[7];
        auto const add     = inst[8];
        auto const ret     = inst[9];
        BOOST_TEST(is_alloca(alloca0, is_i32, constant(1), align_of(4)));
        BOOST_TEST(is_alloca(alloca1, is_i32, constant(1), align_of(4)));
        BOOST_TEST(is_store_of(store0, is_i32, exactly(f->getArg(1)), exactly(alloca0), align_of(4)));
        BOOST_TEST(is_store_of(store1, is_i32, exactly(f->getArg(2)), exactly(alloca1), align_of(4)));
        BOOST_TEST(
            is_direct_call(
                call0,
                exactly(get_function("f1")),
                call_arg(exactly(f->getArg(0)), zext),
                call_arg(exactly(alloca0)),
                call_arg(exactly(alloca1))));
        BOOST_TEST(is_load_of(load0, is_i32, exactly(call0), align_of(4)));
        BOOST_TEST(
            is_direct_call(
                call1,
                exactly(get_function("f1")),
                call_arg(exactly(f->getArg(0)), zext),
                call_arg(exactly(alloca0)),
                call_arg(exactly(alloca1))));
        BOOST_TEST(is_load_of(load1, is_i32, exactly(call1), align_of(4)));
        BOOST_TEST(is_add_of(add, exactly(load0), exactly(load1)));
        BOOST_TEST(is_return_of(ret, exactly(add)));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0023_references/pass_003.depc"));
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(exactly(f->getArg(0)))}));
    }
    {
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(constant(0))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_004.depc"));
    {
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext)) ;
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const alloca = inst[0];
        auto const call1  = inst[1];
        auto const store  = inst[2];
        auto const call2  = inst[3];
        auto const ret    = inst[4];
        BOOST_TEST(is_alloca(alloca, is_i32, constant(1), align_of(4)));
        BOOST_TEST(is_direct_call(call1, exactly(get_function("f1"))));
        BOOST_TEST(is_store_of(store, is_i32, exactly(call1), exactly(alloca), align_of(4)));
        BOOST_TEST(is_direct_call(call2, exactly(get_function("f0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_005.depc"));
    {
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "p", nonnull)}, pointer_to(is_i32)));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(exactly(f->getArg(0)))}));
    }
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "p", nonnull)}, pointer_to(is_i32)));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(exactly(f->getArg(0)))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_006.depc"));
    auto const t = get_struct("t");
    BOOST_TEST(is_struct(t, "t", is_i32, is_i32, pointer_to(is_i32), pointer_to(is_i32)));
    {
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), "p", nonnull),
                    arg_of(pointer_to(is_i32), "q", nonnull)},
            is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        BOOST_TEST(
            is_branch_of(
                blks[0]->getTerminator(),
                cmp_eq(load_of(is_i32, exactly(f->getArg(0)), align_of(4)), constant(0)),
                exactly(blks[1]),
                exactly(blks[2])));
        BOOST_TEST(is_return_of(blks[1]->getTerminator(), load_of(is_i32, exactly(f->getArg(1)), align_of(4))));
        BOOST_TEST(is_return_of(blks[2]->getTerminator(), load_of(is_i32, exactly(f->getArg(0)), align_of(4))));
    }
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), "v", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(get_function("f0")),
                    call_arg(
                        gep_of(
                            exactly(t),
                            exactly(f->getArg(0)),
                            constant(0),
                            constant(0))),
                    call_arg(
                        gep_of(
                            exactly(t),
                            exactly(f->getArg(0)),
                            constant(0),
                            constant(1))))));
    }
    {
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(exactly(t)), "v", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(get_function("f0")),
                    call_arg(
                        gep_of(
                            exactly(t),
                            exactly(f->getArg(0)),
                            constant(0),
                            constant(0))),
                    call_arg(
                        gep_of(
                            exactly(t),
                            exactly(f->getArg(0)),
                            constant(0),
                            constant(1))))));
    }
    {
        auto const f = get_function("g0");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_ptr_to(is_i32),
                    arg_of(pointer_to(pointer_to(is_i32)), "p", nonnull),
                    arg_of(pointer_to(pointer_to(is_i32)), "q", nonnull)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        BOOST_TEST(
            is_branch_of(
                blks[0]->getTerminator(),
                cmp_eq(
                    load_of(
                        is_i32,
                        gep_of(is_i32, load_of(pointer_to(is_i32), exactly(f->getArg(1)), align_of(8)), constant(0)),
                        align_of(4)),
                    constant(0)),
                exactly(blks[1]),
                exactly(blks[2])));
        {
            auto const inst = get_instructions(*blks[1]);
            BOOST_TEST_REQUIRE(inst.size() == 5ul);
            auto const load = inst[0];
            auto const dst  = inst[1];
            auto const src  = inst[2];
            auto const cpy  = inst[3];
            auto const ret  = inst[4];
            BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(f->getArg(2)), align_of(8)));
            BOOST_TEST(is_bitcast_of(dst, exactly(f->getArg(0)), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
            auto const attrs = std::vector{llvm::Attribute::Alignment};
            auto const align = llvm::Align(4);
            BOOST_TEST(
                is_direct_call(
                    cpy,
                    is_memcpy,
                    call_arg(exactly(dst), attrs, align),
                    call_arg(exactly(src), attrs, align),
                    call_arg(constant(12)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
        {
            auto const inst = get_instructions(*blks[2]);
            BOOST_TEST_REQUIRE(inst.size() == 5ul);
            auto const load = inst[0];
            auto const dst  = inst[1];
            auto const src  = inst[2];
            auto const cpy  = inst[3];
            auto const ret  = inst[4];
            BOOST_TEST(is_load_of(load, pointer_to(is_i32), exactly(f->getArg(1)), align_of(8)));
            BOOST_TEST(is_bitcast_of(dst, exactly(f->getArg(0)), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(load), pointer_to(is_i32), pointer_to(is_i8)));
            auto const attrs = std::vector{llvm::Attribute::Alignment};
            auto const align = llvm::Align(4);
            BOOST_TEST(
                is_direct_call(
                    cpy,
                    is_memcpy,
                    call_arg(exactly(dst), attrs, align),
                    call_arg(exactly(src), attrs, align),
                    call_arg(constant(12)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
    }
    {
        auto const f = get_function("g1");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{ret_ptr_to(is_i32), arg_of(pointer_to(exactly(t)), "v", nonnull)}, is_void));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 4ul);
        BOOST_TEST(is_gep_of(inst[0], exactly(t), exactly(f->getArg(1)), constant(0), constant(2)));
        BOOST_TEST(is_gep_of(inst[1], exactly(t), exactly(f->getArg(1)), constant(0), constant(3)));
        BOOST_TEST(
            is_direct_call(
                inst[2],
                exactly(get_function("g0")),
                call_arg(exactly(f->getArg(0))),
                call_arg(exactly(inst[0])),
                call_arg(exactly(inst[1]))));
        BOOST_TEST(is_return_of_void(inst[3]));
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_007.depc"));
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 6ul);
        auto const alloca   = inst[0];
        auto const store    = inst[1];
        auto const call1    = inst[2];
        auto const call2    = inst[3];
        auto const add      = inst[4];
        auto const ret      = inst[5];
        BOOST_TEST(is_alloca(alloca, is_i32, constant(1), align_of(4)));
        BOOST_TEST(is_store_of(store, is_i32, exactly(f->getArg(0)), exactly(alloca), align_of(4)));
        BOOST_TEST(is_direct_call(call1, exactly(get_function("f0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_direct_call(call2, exactly(get_function("f0")), call_arg(exactly(alloca))));
        BOOST_TEST(is_add_of(add, exactly(call1), exactly(call2)));
        BOOST_TEST(is_return_of(ret, exactly(add)));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_008.depc"));
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "v", nonnull)}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 4ul);
        auto const gep1 = inst[0];
        auto const gep2 = inst[1];
        auto const call = inst[2];
        auto const ret  = inst[3];
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(f->getArg(0)), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(f->getArg(0)), constant(1)));
        BOOST_TEST(
            is_direct_call(
                call,
                exactly(get_function("f0")),
                call_arg(exactly(gep1)),
                call_arg(exactly(gep2))));
        BOOST_TEST(is_return_of(ret, exactly(call)));
    }
    {
        auto const tuple_type = struct_of(is_i32, is_i32);
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "v", nonnull)}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 4ul);
        auto const gep1 = inst[0];
        auto const gep2 = inst[1];
        auto const call = inst[2];
        auto const ret  = inst[3];
        BOOST_TEST(is_gep_of(gep1, tuple_type, exactly(f->getArg(0)), constant(0), constant(0)));
        BOOST_TEST(is_gep_of(gep2, tuple_type, exactly(f->getArg(0)), constant(0), constant(1)));
        BOOST_TEST(
            is_direct_call(
                call,
                exactly(get_function("f0")),
                call_arg(exactly(gep1)),
                call_arg(exactly(gep2))));
        BOOST_TEST(is_return_of(ret, exactly(call)));
    }
    {
        auto const tuple_type = struct_of(is_i64, pointer_to(is_i32));
        auto const f = get_function("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "v", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(get_function("f0")),
                    call_arg(
                        gep_of(
                            is_i32,
                            load_of(
                                pointer_to(is_i32),
                                gep_of(tuple_type, exactly(f->getArg(0)), constant(0), constant(1)),
                                align_of(8)),
                            constant(0))),
                    call_arg(
                        gep_of(
                            is_i32,
                            load_of(
                                pointer_to(is_i32),
                                gep_of(tuple_type, exactly(f->getArg(0)), constant(0), constant(1)),
                                align_of(8)),
                            constant(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_009)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_009.depc"));
    {
        auto const f = get_function("zero");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(constant(0))}));
    }
    {
        auto const f = get_function("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(constant(0))}));
    }
    {
        auto const f = get_function("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, pointer_to(fnptr_type(std::tuple{}, is_i32))));
        BOOST_TEST(
            is_block_of(
                f->getEntryBlock(),
                std::tuple{return_of(exactly(pass_result.value()->getGlobalVariable("$_ref_g", true)))}));
    }
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, pointer_to(fnptr_type(std::tuple{}, is_i32))));
        BOOST_TEST(
            is_block_of(
                f->getEntryBlock(),
                std::tuple{return_of(exactly(pass_result.value()->getGlobalVariable("$_ref_g", true)))}));
    }
    {
        auto const f = get_function("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, pointer_to(pointer_to(is_i8))));
        BOOST_TEST(
            is_block_of(
                f->getEntryBlock(),
                std::tuple{return_of(exactly(pass_result.value()->getGlobalVariable("$_strref_", true)))}));
    }
    {
        auto const f = get_function("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, pointer_to(pointer_to(is_i8))));
        BOOST_TEST(
            is_block_of(
                f->getEntryBlock(),
                std::tuple{return_of(exactly(pass_result.value()->getGlobalVariable("$_strref_.2", true)))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_010)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_010.depc"));
    {
        auto const f = get_function("g");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(pointer_to(is_i32)), "p", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(
                    is_i32,
                    gep_of(
                        is_i32,
                        load_of(pointer_to(is_i32), exactly(f->getArg(0)), align_of(8)),
                        constant(0)),
                    align_of(4))));
    }
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "x", nonnull)}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 4ul);
        auto const alloca = inst[0];
        auto const store  = inst[1];
        auto const call   = inst[2];
        auto const ret    = inst[3];
        BOOST_TEST(is_alloca(alloca, pointer_to(is_i32), constant(1), align_of(8)));
        BOOST_TEST(is_store_of(store, pointer_to(is_i32), exactly(f->getArg(0)), exactly(alloca), align_of(8)));
        BOOST_TEST(is_direct_call(call, exactly(get_function("g")), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call)));
    }
    {
        auto const f = get_function("f2");
        auto const tuple_type = struct_of(is_i64, pointer_to(is_i32));
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(tuple_type), "x", nonnull)}, is_i32, sext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(get_function("g")),
                    call_arg(gep_of(tuple_type, exactly(f->getArg(0)), constant(0), constant(1))))));
    }
    {
        auto const f = get_function("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 6ul);
        auto const alloca_v = inst[0];
        auto const alloca_p = inst[1];
        auto const call1    = inst[2];
        auto const store    = inst[3];
        auto const call2    = inst[4];
        auto const ret      = inst[5];
        BOOST_TEST(is_alloca(alloca_v, is_i32, constant(3), align_of(4)));
        BOOST_TEST(is_alloca(alloca_p, pointer_to(is_i32), constant(1), align_of(8)));
        BOOST_TEST(is_direct_call(call1, exactly(get_function("zero")), call_arg(exactly(alloca_v))));
        BOOST_TEST(is_store_of(store, pointer_to(is_i32), exactly(alloca_v), exactly(alloca_p), align_of(8)));
        BOOST_TEST(is_direct_call(call2, exactly(get_function("g")), call_arg(exactly(alloca_p))));
        BOOST_TEST(is_return_of(ret, exactly(call2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_011)
{
    BOOST_TEST_REQUIRE(pass("0023_references/pass_011.depc"));
    BOOST_TEST(get_function("::double_neg_intro"));
    BOOST_TEST(get_function("g"));
    {
        auto const f = get_function("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        auto const ref = pass_result.value()->getGlobalVariable("$_ref_::double_neg_intro", true);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 2ul);
        BOOST_TEST(is_direct_call(inst[0], exactly(get_function("g")), call_arg(exactly(ref))));
        BOOST_TEST(is_return_of(inst[1], exactly(inst[0])));
    }
}

BOOST_AUTO_TEST_CASE(pass_012) { BOOST_TEST(pass("0023_references/pass_012.depc")); }
BOOST_AUTO_TEST_CASE(pass_013) { BOOST_TEST(pass("0023_references/pass_013.depc")); }
BOOST_AUTO_TEST_CASE(pass_014) { BOOST_TEST(pass("0023_references/pass_014.depc")); }
BOOST_AUTO_TEST_CASE(pass_015) { BOOST_TEST(pass("0023_references/pass_015.depc")); }

BOOST_AUTO_TEST_SUITE_END()
