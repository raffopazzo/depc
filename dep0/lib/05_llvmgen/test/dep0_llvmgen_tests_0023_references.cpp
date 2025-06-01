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
    BOOST_TEST_REQUIRE(pass_result.value()->getFunction("puts"));
    BOOST_TEST_REQUIRE(pass_result.value()->getFunction("f0"));
    {
        auto const f = get_function("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, struct_of()));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(is_zeroinitializer)}));
    }
    for (auto const name: {"f2", "f3", "f4", "f5"})
    {
        BOOST_TEST_CONTEXT(name);
        auto const f = get_function(name);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, struct_of()));
        BOOST_TEST(is_block_of(f->getEntryBlock(), std::tuple{return_of(is_zeroinitializer)}));
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

BOOST_AUTO_TEST_SUITE_END()
