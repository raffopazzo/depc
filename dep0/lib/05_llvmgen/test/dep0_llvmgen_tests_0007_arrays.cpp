#define BOOST_TEST_MODULE dep0_llvmgen_tests_0007_arrays
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

template <Predicate<llvm::Type> F>
auto ret_arg_of(F&& f)
{
    return arg_of(pointer_to(std::forward<F>(f)), std::nullopt, {llvm::Attribute::NonNull, llvm::Attribute::StructRet});
}

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0007_arrays, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("values");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i32)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 7ul);
        auto const gep1   = inst[0];
        auto const store1 = inst[1];
        auto const gep2   = inst[2];
        auto const store2 = inst[3];
        auto const gep3   = inst[4];
        auto const store3 = inst[5];
        auto const ret    = inst[6];
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(gep2), align_of(4)));
        BOOST_TEST(is_store_of(store3, is_i32, constant(3), exactly(gep3), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("empty_of_int");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i32)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST(bb.size() == 1ul);
        BOOST_TEST(is_return_of_void(bb.getTerminator()));
    }
    {
        auto const f = pass_result.value()->getFunction("empty_of_bool");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i1)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST(bb.size() == 1ul);
        BOOST_TEST(is_return_of_void(bb.getTerminator()));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("discard");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i64, "n", zext),
                    arg_of(pointer_to(is_i32), std::nullopt, nonnull)},
                is_i8));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() == 1ul);
        BOOST_TEST(is_return_of(bb.front(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("nothing");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 3ul);
        auto const alloca = inst[0];
        auto const call   = inst[1];
        auto const ret    = inst[2];
        BOOST_TEST(is_alloca(alloca, is_i32, constant(0), align_of(4)));
        BOOST_TEST(
            is_direct_call(
                call,
                exactly(pass_result.value()->getFunction("discard")),
                call_arg(constant(0), zext),
                call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("values");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i32)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        auto const gep1   = inst[0];
        auto const store1 = inst[1];
        auto const gep2   = inst[2];
        auto const store2 = inst[3];
        auto const ret    = inst[4];
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(gep2), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("first");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(0ul)), constant(0)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("second");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(0ul)), constant(1)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("third");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(0ul)), constant(2)), align_of(4))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_005.depc"));
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_006.depc"));
    {
        auto const f = pass_result.value()->getFunction("sum");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = exactly(f->getArg(0ul));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(
                    add_of(
                        load_of(is_i32, gep_of(is_i32, xs, constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, xs, constant(1)), align_of(4))),
                    load_of(is_i32, gep_of(is_i32, xs, constant(2)), align_of(4)))));
    }
    {
        auto const f = pass_result.value()->getFunction("fifteen");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 9ul);
        auto const alloca = inst[0];
        auto const gep1   = inst[1];
        auto const store1 = inst[2];
        auto const gep2   = inst[3];
        auto const store2 = inst[4];
        auto const gep3   = inst[5];
        auto const store3 = inst[6];
        auto const call   = inst[7];
        auto const ret    = inst[8];
        BOOST_TEST(is_alloca(alloca, is_i32, constant(3), align_of(4)));
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(alloca), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(alloca), constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i32, exactly(alloca), constant(2)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(3), exactly(gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(5), exactly(gep2), align_of(4)));
        BOOST_TEST(is_store_of(store3, is_i32, constant(7), exactly(gep3), align_of(4)));
        BOOST_TEST(is_direct_call(call, exactly(pass_result.value()->getFunction("sum")), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call)));
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_007.depc"));
    {
        auto const f = pass_result.value()->getFunction("unit");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("three_units");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i8)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 10ul);
        auto const gep1   = inst[0];
        auto const call1  = inst[1];
        auto const store1 = inst[2];
        auto const gep2   = inst[3];
        auto const call2  = inst[4];
        auto const store2 = inst[5];
        auto const gep3   = inst[6];
        auto const call3  = inst[7];
        auto const store3 = inst[8];
        auto const ret    = inst[9];
        auto const ret_arg = f->getArg(0ul);
        auto const unit = pass_result.value()->getFunction("unit");
        BOOST_TEST(is_gep_of(gep1, is_i8, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i8, exactly(ret_arg), constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i8, exactly(ret_arg), constant(2)));
        BOOST_TEST(is_direct_call(call1, exactly(unit)));
        BOOST_TEST(is_direct_call(call2, exactly(unit)));
        BOOST_TEST(is_direct_call(call3, exactly(unit)));
        BOOST_TEST(is_store_of(store1, is_i8, exactly(call1), exactly(gep1), align_of(1)));
        BOOST_TEST(is_store_of(store2, is_i8, exactly(call2), exactly(gep2), align_of(1)));
        BOOST_TEST(is_store_of(store3, is_i8, exactly(call3), exactly(gep3), align_of(1)));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_008.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i32)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 15ul);
        auto const gep_row_0   = inst[0];
        auto const gep_val_0_0 = inst[1];
        auto const store_0_0   = inst[2];
        auto const gep_val_0_1 = inst[3];
        auto const store_0_1   = inst[4];
        auto const gep_val_0_2 = inst[5];
        auto const store_0_2   = inst[6];
        auto const gep_row_1   = inst[7];
        auto const gep_val_1_0 = inst[8];
        auto const store_1_0   = inst[9];
        auto const gep_val_1_1 = inst[10];
        auto const store_1_1   = inst[11];
        auto const gep_val_1_2 = inst[12];
        auto const store_1_2   = inst[13];
        auto const ret         = inst[14];
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep_row_0, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep_row_1, is_i32, exactly(ret_arg), constant(3)));
        BOOST_TEST(is_gep_of(gep_val_0_0, is_i32, exactly(gep_row_0), constant(0)));
        BOOST_TEST(is_gep_of(gep_val_0_1, is_i32, exactly(gep_row_0), constant(1)));
        BOOST_TEST(is_gep_of(gep_val_0_2, is_i32, exactly(gep_row_0), constant(2)));
        BOOST_TEST(is_gep_of(gep_val_1_0, is_i32, exactly(gep_row_1), constant(0)));
        BOOST_TEST(is_gep_of(gep_val_1_1, is_i32, exactly(gep_row_1), constant(1)));
        BOOST_TEST(is_gep_of(gep_val_1_2, is_i32, exactly(gep_row_1), constant(2)));
        BOOST_TEST(is_store_of(store_0_0, is_i32, constant(1), exactly(gep_val_0_0), align_of(4)));
        BOOST_TEST(is_store_of(store_0_1, is_i32, constant(2), exactly(gep_val_0_1), align_of(4)));
        BOOST_TEST(is_store_of(store_0_2, is_i32, constant(3), exactly(gep_val_0_2), align_of(4)));
        BOOST_TEST(is_store_of(store_1_0, is_i32, constant(4), exactly(gep_val_1_0), align_of(4)));
        BOOST_TEST(is_store_of(store_1_1, is_i32, constant(5), exactly(gep_val_1_1), align_of(4)));
        BOOST_TEST(is_store_of(store_1_2, is_i32, constant(6), exactly(gep_val_1_2), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "m", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 8ul);
        auto const gep_row_0    = inst[0];
        auto const gep_val_0_1  = inst[1];
        auto const load_val_0_1 = inst[2];
        auto const gep_row_1    = inst[3];
        auto const gep_val_1_2  = inst[4];
        auto const load_val_1_2 = inst[5];
        auto const add          = inst[6];
        auto const ret          = inst[7];
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep_row_0, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep_row_1, is_i32, exactly(ret_arg), constant(3)));
        BOOST_TEST(is_gep_of(gep_val_0_1, is_i32, exactly(gep_row_0), constant(1)));
        BOOST_TEST(is_gep_of(gep_val_1_2, is_i32, exactly(gep_row_1), constant(2)));
        BOOST_TEST(is_load_of(load_val_0_1, is_i32, exactly(gep_val_0_1), align_of(4)));
        BOOST_TEST(is_load_of(load_val_1_2, is_i32, exactly(gep_val_1_2), align_of(4)));
        BOOST_TEST(is_add_of(add, exactly(load_val_0_1), exactly(load_val_1_2)));
        BOOST_TEST(is_return_of(ret, exactly(add)));
    }
}

BOOST_AUTO_TEST_CASE(pass_009)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_009.depc"));
    {
        auto const f = pass_result.value()->getFunction("sum");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = exactly(f->getArg(0ul));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(
                    add_of(
                        load_of(is_i32, gep_of(is_i32, xs, constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, xs, constant(1)), align_of(4))),
                    load_of(is_i32, gep_of(is_i32, xs, constant(2)), align_of(4)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "m", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("sum")),
                    call_arg(gep_of(is_i32, exactly(f->getArg(0ul)), constant(6))))));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "m", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const m = exactly(f->getArg(0ul));
        auto const sum = exactly(pass_result.value()->getFunction("sum"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(
                    add_of(
                        add_of(
                            direct_call_of(sum, call_arg(gep_of(is_i32, m, constant(0)))),
                            direct_call_of(sum, call_arg(gep_of(is_i32, m, constant(3))))),
                        direct_call_of(sum, call_arg(gep_of(is_i32, m, constant(6))))),
                    direct_call_of(sum, call_arg(gep_of(is_i32, m, constant(9)))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_010)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_010.depc"));
    {
        auto const f = pass_result.value()->getFunction("values");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i32)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 7ul);
        auto const gep1   = inst[0];
        auto const store1 = inst[1];
        auto const gep2   = inst[2];
        auto const store2 = inst[3];
        auto const gep3   = inst[4];
        auto const store3 = inst[5];
        auto const ret    = inst[6];
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(gep2), align_of(4)));
        BOOST_TEST(is_store_of(store3, is_i32, constant(3), exactly(gep3), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = pass_result.value()->getFunction("six");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 15ul);
        auto const alloca1 = inst[0];
        auto const call1   = inst[1];
        auto const gep1    = inst[2];
        auto const load1   = inst[3];
        auto const alloca2 = inst[4];
        auto const call2   = inst[5];
        auto const gep2    = inst[6];
        auto const load2   = inst[7];
        auto const add1    = inst[8];
        auto const alloca3 = inst[9];
        auto const call3   = inst[10];
        auto const gep3    = inst[11];
        auto const load3   = inst[12];
        auto const add2    = inst[13];
        auto const ret     = inst[14];
        auto const values = exactly(pass_result.value()->getFunction("values"));
        BOOST_TEST(is_alloca(alloca1, is_i32, constant(3), align_of(4)));
        BOOST_TEST(is_alloca(alloca2, is_i32, constant(3), align_of(4)));
        BOOST_TEST(is_alloca(alloca3, is_i32, constant(3), align_of(4)));
        BOOST_TEST(is_direct_call(call1, values, call_arg(exactly(alloca1))));
        BOOST_TEST(is_direct_call(call2, values, call_arg(exactly(alloca2))));
        BOOST_TEST(is_direct_call(call3, values, call_arg(exactly(alloca3))));
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(alloca1), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(alloca2), constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i32, exactly(alloca3), constant(2)));
        BOOST_TEST(is_load_of(load1, is_i32, exactly(gep1), align_of(4)));
        BOOST_TEST(is_load_of(load2, is_i32, exactly(gep2), align_of(4)));
        BOOST_TEST(is_load_of(load3, is_i32, exactly(gep3), align_of(4)));
        BOOST_TEST(is_add_of(add1, exactly(load1), exactly(load2)));
        BOOST_TEST(is_add_of(add2, exactly(add1), exactly(load3)));
        BOOST_TEST(is_return_of(ret, exactly(add2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_011)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_011.depc"));
    {
        auto const f = pass_result.value()->getFunction("transform_add");
        auto const fn_ptr = fnptr_type(std::tuple{is_i32}, is_i32);
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), "xs", nonnull),
                    arg_of(pointer_to(fn_ptr), "fs", nonnull)},
                is_i32,
                sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = exactly(f->getArg(0ul));
        auto const fs = exactly(f->getArg(1ul));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(
                    add_of(
                        indirect_call_of(
                            load_of(fn_ptr, gep_of(fn_ptr, fs, constant(0)), align_of(8)),
                            call_arg(load_of(is_i32, gep_of(is_i32, xs, constant(0)), align_of(4)), sext)),
                        indirect_call_of(
                            load_of(fn_ptr, gep_of(fn_ptr, fs, constant(1)), align_of(8)),
                            call_arg(load_of(is_i32, gep_of(is_i32, xs, constant(1)), align_of(4)), sext))),
                    indirect_call_of(
                        load_of(fn_ptr, gep_of(fn_ptr, fs, constant(2)), align_of(8)),
                        call_arg(load_of(is_i32, gep_of(is_i32, xs, constant(2)), align_of(4)), sext)))));
    }
    {
        auto const f = pass_result.value()->getFunction("identity");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0ul))));
    }
    {
        auto const f = pass_result.value()->getFunction("plus_one");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(exactly(f->getArg(0ul)), constant(1))));
    }
    {
        auto const f = pass_result.value()->getFunction("plus_two");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("eighteen");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 16ul);
        auto const alloca_xs  = inst[0];
        auto const gep_xs_0   = inst[1];
        auto const store_xs_0 = inst[2];
        auto const gep_xs_1   = inst[3];
        auto const store_xs_1 = inst[4];
        auto const gep_xs_2   = inst[5];
        auto const store_xs_2 = inst[6];
        auto const alloca_fs  = inst[7];
        auto const gep_fs_0   = inst[8];
        auto const store_fs_0 = inst[9];
        auto const gep_fs_1   = inst[10];
        auto const store_fs_1 = inst[11];
        auto const gep_fs_2   = inst[12];
        auto const store_fs_2 = inst[13];
        auto const call       = inst[14];
        auto const ret        = inst[15];
        auto const fn_ptr = fnptr_type(std::tuple{is_i32}, is_i32);
        auto const identity = exactly(pass_result.value()->getFunction("identity"));
        auto const plus_one = exactly(pass_result.value()->getFunction("plus_one"));
        auto const plus_two = exactly(pass_result.value()->getFunction("plus_two"));
        auto const transform_add = exactly(pass_result.value()->getFunction("transform_add"));
        BOOST_TEST(is_alloca(alloca_xs, is_i32, constant(3), align_of(4)));
        BOOST_TEST(is_alloca(alloca_fs, fn_ptr, constant(3), align_of(8)));
        BOOST_TEST(is_gep_of(gep_xs_0, is_i32, exactly(alloca_xs), constant(0)));
        BOOST_TEST(is_gep_of(gep_xs_1, is_i32, exactly(alloca_xs), constant(1)));
        BOOST_TEST(is_gep_of(gep_xs_2, is_i32, exactly(alloca_xs), constant(2)));
        BOOST_TEST(is_gep_of(gep_fs_0, fn_ptr, exactly(alloca_fs), constant(0)));
        BOOST_TEST(is_gep_of(gep_fs_1, fn_ptr, exactly(alloca_fs), constant(1)));
        BOOST_TEST(is_gep_of(gep_fs_2, fn_ptr, exactly(alloca_fs), constant(2)));
        BOOST_TEST(is_store_of(store_xs_0, is_i32, constant(3), exactly(gep_xs_0), align_of(4)));
        BOOST_TEST(is_store_of(store_xs_1, is_i32, constant(5), exactly(gep_xs_1), align_of(4)));
        BOOST_TEST(is_store_of(store_xs_2, is_i32, constant(7), exactly(gep_xs_2), align_of(4)));
        BOOST_TEST(is_store_of(store_fs_0, fn_ptr, identity, exactly(gep_fs_0), align_of(8)));
        BOOST_TEST(is_store_of(store_fs_1, fn_ptr, plus_one, exactly(gep_fs_1), align_of(8)));
        BOOST_TEST(is_store_of(store_fs_2, fn_ptr, plus_two, exactly(gep_fs_2), align_of(8)));
        BOOST_TEST(is_direct_call(call, transform_add, call_arg(exactly(alloca_xs)), call_arg(exactly(alloca_fs))));
        BOOST_TEST(is_return_of(ret, exactly(call)));
    }
}

BOOST_AUTO_TEST_CASE(pass_011_normalized)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_011.depc"));
    {
        auto const f = pass_result.value()->getFunction("eighteen");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(18)));
    }
}

BOOST_AUTO_TEST_CASE(pass_012)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_012.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    auto const exactly_f = exactly(pass_result.value()->getFunction("f"));
    auto const exactly_g = exactly(pass_result.value()->getFunction("g"));
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext)}, is_i1, zext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(f->getArg(0ul)), exactly(then0), exactly(else0)));
        BOOST_TEST(is_return_of(then0->getTerminator(), constant(false)));
        BOOST_TEST(is_return_of(else0->getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("select");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "which", zext)}, fnptr_type(std::tuple{}, is_i32)));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(f->getArg(0ul)), exactly(then0), exactly(else0)));
        BOOST_TEST(is_return_of(then0->getTerminator(), exactly_f));
        BOOST_TEST(is_return_of(else0->getTerminator(), exactly_g));
    }
    {
        auto const f = pass_result.value()->getFunction("choose");
        auto const fn_ptr = fnptr_type(std::tuple{}, is_i32);
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(fn_ptr), arg_of(is_i1, "which", zext)}, is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 10ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const cont0 = blks[3];
        auto const then2 = blks[4];
        auto const else3 = blks[5];
        auto const cont4 = blks[6];
        auto const then5 = blks[7];
        auto const else6 = blks[8];
        auto const cont7 = blks[9];
        auto const ret_arg = exactly(f->getArg(0ul));
        auto const which = exactly(f->getArg(1ul));
        auto const [gep0, temp_bool] = [&]
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 3ul);
            auto const gep0      = inst[0];
            auto const temp_bool = inst[1];
            auto const br        = inst[2];
            BOOST_TEST(is_gep_of(gep0, fn_ptr, ret_arg, constant(0)));
            BOOST_TEST(is_alloca(temp_bool, is_i1, constant(1), align_of(1)));
            BOOST_TEST(is_branch_of(br, which, exactly(then0), exactly(else0)));
            return std::tuple{gep0, temp_bool};
        }();
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], fn_ptr, exactly_f, exactly(gep0), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont0)));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], fn_ptr, exactly_g, exactly(gep0), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont0)));
        }
        auto const gep1 = [&]
        {
            auto const inst = get_instructions(*cont0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_gep_of(inst[0], fn_ptr, ret_arg, constant(1)));
            BOOST_TEST(is_branch_of(inst[1], which, exactly(then2), exactly(else3)));
            return inst[0];
        }();
        {
            auto const inst = get_instructions(*then2);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], is_i1, constant(false), exactly(temp_bool), align_of(1)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont4)));
        }
        {
            auto const inst = get_instructions(*else3);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], is_i1, constant(true), exactly(temp_bool), align_of(1)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont4)));
        }
        {
            auto const inst = get_instructions(*cont4);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_load_of(inst[0], is_i1, exactly(temp_bool), align_of(1)));
            BOOST_TEST(is_branch_of(inst[1], exactly(inst[0]), exactly(then5), exactly(else6)));
        }
        {
            auto const inst = get_instructions(*then5);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], fn_ptr, exactly_f, exactly(gep1), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont7)));
        }
        {
            auto const inst = get_instructions(*else6);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], fn_ptr, exactly_g, exactly(gep1), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont7)));
        }
        {
            BOOST_TEST_REQUIRE(cont7->size() == 1ul);
            BOOST_TEST(is_return_of_void(cont7->getTerminator()));
        }
    }
}

BOOST_AUTO_TEST_CASE(pass_013)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_013.depc"));
    {
        auto const f = pass_result.value()->getFunction("minus_one");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
}

BOOST_AUTO_TEST_CASE(pass_014)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_014.depc"));
    {
        auto const f = pass_result.value()->getFunction("zeros");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i32)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 7ul);
        auto const gep1   = inst[0];
        auto const store1 = inst[1];
        auto const gep2   = inst[2];
        auto const store2 = inst[3];
        auto const gep3   = inst[4];
        auto const store3 = inst[5];
        auto const ret    = inst[6];
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(0), exactly(gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(0), exactly(gep2), align_of(4)));
        BOOST_TEST(is_store_of(store3, is_i32, constant(0), exactly(gep3), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
    auto const zeros = exactly(pass_result.value()->getFunction("zeros"));
    {
        auto const f = pass_result.value()->getFunction("passthrough");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{ret_arg_of(is_i32)}, is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 2ul);
        BOOST_TEST(is_direct_call(inst[0], zeros, call_arg(exactly(f->getArg(0ul)))));
        BOOST_TEST(is_return_of_void(inst[1]));
    }
    {
        auto const f = pass_result.value()->getFunction("values");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_arg_of(is_i32),
                    arg_of(is_i1, "which", zext)},
                is_void));
        auto blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const ret_arg = f->getArg(0ul);
        {
            BOOST_TEST_REQUIRE(entry->size() == 1ul);
            BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(f->getArg(1ul)), exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_direct_call(inst[0], zeros, call_arg(exactly(ret_arg))));
            BOOST_TEST(is_return_of_void(inst[1]));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 7ul);
            auto const gep1   = inst[0];
            auto const store1 = inst[1];
            auto const gep2   = inst[2];
            auto const store2 = inst[3];
            auto const gep3   = inst[4];
            auto const store3 = inst[5];
            auto const ret    = inst[6];
            BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
            BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
            BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
            BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(gep1), align_of(4)));
            BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(gep2), align_of(4)));
            BOOST_TEST(is_store_of(store3, is_i32, constant(3), exactly(gep3), align_of(4)));
            BOOST_TEST(is_return_of_void(ret));
        }
    }
    {
        auto const f = pass_result.value()->getFunction("select");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_arg_of(is_i32),
                    arg_of(is_i1, "which", zext),
                    arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const ret_arg = f->getArg(0ul);
        auto const which = f->getArg(1ul);
        auto const xs = f->getArg(2ul);
        {
            BOOST_TEST_REQUIRE(entry->size() == 1ul);
            BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(which), exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_direct_call(inst[0], zeros, call_arg(exactly(ret_arg))));
            BOOST_TEST(is_return_of_void(inst[1]));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 4ul);
            auto const dst    = inst[0];
            auto const src    = inst[1];
            auto const memcpy = inst[2];
            auto const ret    = inst[3];
            BOOST_TEST(is_bitcast_of(dst, exactly(ret_arg), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(xs), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(
                is_direct_call(
                    memcpy,
                    exactly(pass_result.value()->getFunction(llvm_memcpy_name)),
                    call_arg(exactly(dst), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(src), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(constant(12)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
    }
    {
        auto const f = pass_result.value()->getFunction("xs_or_ys");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_arg_of(is_i32),
                    arg_of(is_i1, "which", zext),
                    arg_of(pointer_to(is_i32), "xs", nonnull),
                    arg_of(pointer_to(is_i32), "ys", nonnull)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const ret_arg = f->getArg(0ul);
        auto const which = f->getArg(1ul);
        auto const xs = f->getArg(2ul);
        auto const ys = f->getArg(3ul);
        {
            BOOST_TEST_REQUIRE(entry->size() == 1ul);
            BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(which), exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 4ul);
            auto const dst    = inst[0];
            auto const src    = inst[1];
            auto const memcpy = inst[2];
            auto const ret    = inst[3];
            BOOST_TEST(is_bitcast_of(dst, exactly(ret_arg), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(xs), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(
                is_direct_call(
                    memcpy,
                    exactly(pass_result.value()->getFunction(llvm_memcpy_name)),
                    call_arg(exactly(dst), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(src), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(constant(12)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 4ul);
            auto const dst    = inst[0];
            auto const src    = inst[1];
            auto const memcpy = inst[2];
            auto const ret    = inst[3];
            BOOST_TEST(is_bitcast_of(dst, exactly(ret_arg), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(ys), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(
                is_direct_call(
                    memcpy,
                    exactly(pass_result.value()->getFunction(llvm_memcpy_name)),
                    call_arg(exactly(dst), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(src), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(constant(12)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
    }
}

BOOST_AUTO_TEST_CASE(pass_015)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_015.depc"));
    {
        auto const f = pass_result.value()->getFunction("xs_or_ys");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_arg_of(is_i32),
                    arg_of(is_i1, "which", zext),
                    arg_of(is_i64, "n", zext),
                    arg_of(pointer_to(is_i32), "xs", nonnull),
                    arg_of(pointer_to(is_i32), "ys", nonnull)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const ret_arg = f->getArg(0ul);
        auto const which = f->getArg(1ul);
        auto const n = f->getArg(2ul);
        auto const xs = f->getArg(3ul);
        auto const ys = f->getArg(4ul);
        {
            BOOST_TEST_REQUIRE(entry->size() == 1ul);
            BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(which), exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 5ul);
            auto const mul    = inst[0];
            auto const dst    = inst[1];
            auto const src    = inst[2];
            auto const memcpy = inst[3];
            auto const ret    = inst[4];
            BOOST_TEST(is_mul_of(mul, exactly(n), constant(4)));
            BOOST_TEST(is_bitcast_of(dst, exactly(ret_arg), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(xs), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(
                is_direct_call(
                    memcpy,
                    exactly(pass_result.value()->getFunction(llvm_memcpy_name)),
                    call_arg(exactly(dst), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(src), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(mul)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 5ul);
            auto const mul    = inst[0];
            auto const dst    = inst[1];
            auto const src    = inst[2];
            auto const memcpy = inst[3];
            auto const ret    = inst[4];
            BOOST_TEST(is_mul_of(mul, exactly(n), constant(4)));
            BOOST_TEST(is_bitcast_of(dst, exactly(ret_arg), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(ys), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(
                is_direct_call(
                    memcpy,
                    exactly(pass_result.value()->getFunction(llvm_memcpy_name)),
                    call_arg(exactly(dst), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(src), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(mul)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
    }
    {
        auto const f = pass_result.value()->getFunction("xs_or_ys_2");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_arg_of(is_i32),
                    arg_of(is_i1, "which", zext),
                    arg_of(pointer_to(is_i32), "xs", nonnull),
                    arg_of(pointer_to(is_i32), "ys", nonnull)},
                is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 2ul);
        BOOST_TEST(
            is_direct_call(
                inst[0],
                exactly(pass_result.value()->getFunction("xs_or_ys")),
                call_arg(exactly(f->getArg(0ul))),
                call_arg(exactly(f->getArg(1ul)), zext),
                call_arg(constant(2), zext),
                call_arg(exactly(f->getArg(2ul))),
                call_arg(exactly(f->getArg(3ul)))));
        BOOST_TEST(is_return_of_void(inst[1]));
    }
}

BOOST_AUTO_TEST_CASE(pass_016_without_normalization)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_016.depc"));
    {
        auto const f = pass_result.value()->getFunction("last");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(0ul)), constant(2)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("count_3");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_arg_of(is_i32),
                    arg_of(is_i1, "start_from_zero", zext)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const ret_arg = f->getArg(0ul);
        auto const start_from_zero = f->getArg(1ul);
        BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(start_from_zero), exactly(then0), exactly(else0)));
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 7ul);
            auto const gep1   = inst[0];
            auto const store1 = inst[1];
            auto const gep2   = inst[2];
            auto const store2 = inst[3];
            auto const gep3   = inst[4];
            auto const store3 = inst[5];
            auto const ret    = inst[6];
            BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
            BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
            BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
            BOOST_TEST(is_store_of(store1, is_i32, constant(0), exactly(gep1), align_of(4)));
            BOOST_TEST(is_store_of(store2, is_i32, constant(1), exactly(gep2), align_of(4)));
            BOOST_TEST(is_store_of(store3, is_i32, constant(2), exactly(gep3), align_of(4)));
            BOOST_TEST(is_return_of_void(ret));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 7ul);
            auto const gep1   = inst[0];
            auto const store1 = inst[1];
            auto const gep2   = inst[2];
            auto const store2 = inst[3];
            auto const gep3   = inst[4];
            auto const store3 = inst[5];
            auto const ret    = inst[6];
            BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
            BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
            BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
            BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(gep1), align_of(4)));
            BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(gep2), align_of(4)));
            BOOST_TEST(is_store_of(store3, is_i32, constant(3), exactly(gep3), align_of(4)));
            BOOST_TEST(is_return_of_void(ret));
        }
    }
    {
        auto const f = pass_result.value()->getFunction("two_or_three");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "start_from_zero", zext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 4ul);
        auto const alloca = inst[0];
        auto const call1  = inst[1];
        auto const call2  = inst[2];
        auto const ret    = inst[3];
        BOOST_TEST(is_alloca(alloca, is_i32, constant(3), align_of(4)));
        BOOST_TEST(
            is_direct_call(
                call1,
                exactly(pass_result.value()->getFunction("count_3")),
                call_arg(exactly(alloca)),
                call_arg(exactly(f->getArg(0ul)), zext)));
        BOOST_TEST(
            is_direct_call(
                call2,
                exactly(pass_result.value()->getFunction("last")),
                call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_016_with_normalization)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_016.depc"));
    {
        auto const f = pass_result.value()->getFunction("last");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(0ul)), constant(2)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("count_3");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_arg_of(is_i32),
                    arg_of(is_i1, "start_from_zero", zext)},
                is_void));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const ret_arg = f->getArg(0ul);
        auto const start_from_zero = f->getArg(1ul);
        BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(start_from_zero), exactly(then0), exactly(else0)));
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 7ul);
            auto const gep1   = inst[0];
            auto const store1 = inst[1];
            auto const gep2   = inst[2];
            auto const store2 = inst[3];
            auto const gep3   = inst[4];
            auto const store3 = inst[5];
            auto const ret    = inst[6];
            BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
            BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
            BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
            BOOST_TEST(is_store_of(store1, is_i32, constant(0), exactly(gep1), align_of(4)));
            BOOST_TEST(is_store_of(store2, is_i32, constant(1), exactly(gep2), align_of(4)));
            BOOST_TEST(is_store_of(store3, is_i32, constant(2), exactly(gep3), align_of(4)));
            BOOST_TEST(is_return_of_void(ret));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 7ul);
            auto const gep1   = inst[0];
            auto const store1 = inst[1];
            auto const gep2   = inst[2];
            auto const store2 = inst[3];
            auto const gep3   = inst[4];
            auto const store3 = inst[5];
            auto const ret    = inst[6];
            BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
            BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
            BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
            BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(gep1), align_of(4)));
            BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(gep2), align_of(4)));
            BOOST_TEST(is_store_of(store3, is_i32, constant(3), exactly(gep3), align_of(4)));
            BOOST_TEST(is_return_of_void(ret));
        }
    }
    {
        auto const f = pass_result.value()->getFunction("two_or_three");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "start_from_zero", zext)}, is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 4ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const cont0 = blks[3];
        auto const alloca = [&]
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_alloca(inst[0], is_i32, constant(3), align_of(4)));
            BOOST_TEST(is_branch_of(inst[1], exactly(f->getArg(0ul)), exactly(then0), exactly(else0)));
            return inst[0];
        }();
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 7ul);
            auto const gep1   = inst[0];
            auto const store1 = inst[1];
            auto const gep2   = inst[2];
            auto const store2 = inst[3];
            auto const gep3   = inst[4];
            auto const store3 = inst[5];
            auto const br     = inst[6];
            BOOST_TEST(is_gep_of(gep1, is_i32, exactly(alloca), constant(0)));
            BOOST_TEST(is_gep_of(gep2, is_i32, exactly(alloca), constant(1)));
            BOOST_TEST(is_gep_of(gep3, is_i32, exactly(alloca), constant(2)));
            BOOST_TEST(is_store_of(store1, is_i32, constant(0), exactly(gep1), align_of(4)));
            BOOST_TEST(is_store_of(store2, is_i32, constant(1), exactly(gep2), align_of(4)));
            BOOST_TEST(is_store_of(store3, is_i32, constant(2), exactly(gep3), align_of(4)));
            BOOST_TEST(is_unconditional_branch_to(br, exactly(cont0)));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 7ul);
            auto const gep1   = inst[0];
            auto const store1 = inst[1];
            auto const gep2   = inst[2];
            auto const store2 = inst[3];
            auto const gep3   = inst[4];
            auto const store3 = inst[5];
            auto const br     = inst[6];
            BOOST_TEST(is_gep_of(gep1, is_i32, exactly(alloca), constant(0)));
            BOOST_TEST(is_gep_of(gep2, is_i32, exactly(alloca), constant(1)));
            BOOST_TEST(is_gep_of(gep3, is_i32, exactly(alloca), constant(2)));
            BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(gep1), align_of(4)));
            BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(gep2), align_of(4)));
            BOOST_TEST(is_store_of(store3, is_i32, constant(3), exactly(gep3), align_of(4)));
            BOOST_TEST(is_unconditional_branch_to(br, exactly(cont0)));
        }
        BOOST_TEST(
            is_return_of(
                cont0->getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(alloca), constant(2)), align_of(4))));
    }
}

BOOST_AUTO_TEST_CASE(pass_017)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_017.depc"));
    auto const memcpy_fn = pass_result.value()->getFunction(llvm_memcpy_name);
    BOOST_TEST_REQUIRE(memcpy_fn);
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 11ul);
        auto const alloca      = inst[0];
        auto const gep_tmp_0   = inst[1];
        auto const gep_xs_1    = inst[2];
        auto const load_xs_1   = inst[3];
        auto const store_tmp_0 = inst[4];
        auto const gep_tmp_1   = inst[5];
        auto const gep_xs_0    = inst[6];
        auto const load_xs_0   = inst[7];
        auto const store_tmp_1 = inst[8];
        auto const call        = inst[9];
        auto const ret         = inst[10];
        auto const xs = f->getArg(0ul);
        BOOST_TEST(is_alloca(alloca, is_i32, constant(2), align_of(4)));
        BOOST_TEST(is_gep_of(gep_xs_0, is_i32, exactly(xs), constant(0)));
        BOOST_TEST(is_gep_of(gep_xs_1, is_i32, exactly(xs), constant(1)));
        BOOST_TEST(is_gep_of(gep_tmp_0, is_i32, exactly(alloca), constant(0)));
        BOOST_TEST(is_gep_of(gep_tmp_1, is_i32, exactly(alloca), constant(1)));
        BOOST_TEST(is_load_of(load_xs_0, is_i32, exactly(gep_xs_0), align_of(4)));
        BOOST_TEST(is_load_of(load_xs_1, is_i32, exactly(gep_xs_1), align_of(4)));
        BOOST_TEST(is_store_of(store_tmp_0, is_i32, exactly(load_xs_1), exactly(gep_tmp_0), align_of(4)));
        BOOST_TEST(is_store_of(store_tmp_1, is_i32, exactly(load_xs_0), exactly(gep_tmp_1), align_of(4)));
        BOOST_TEST(is_direct_call(call, exactly(f), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 13ul);
        auto const alloca      = inst[0];
        auto const gep_tmp_0   = inst[1];
        auto const gep_xs_1    = inst[2];
        auto const dst_tmp_0   = inst[3];
        auto const src_xs_1    = inst[4];
        auto const memcpy_0    = inst[5];
        auto const gep_tmp_1   = inst[6];
        auto const gep_xs_0    = inst[7];
        auto const dst_tmp_1   = inst[8];
        auto const src_xs_0    = inst[9];
        auto const memcpy_1    = inst[10];
        auto const call        = inst[11];
        auto const ret         = inst[12];
        auto const xs = f->getArg(0ul);
        BOOST_TEST(is_alloca(alloca, is_i32, constant(4), align_of(4)));
        BOOST_TEST(is_gep_of(gep_xs_0, is_i32, exactly(xs), constant(0)));
        BOOST_TEST(is_gep_of(gep_xs_1, is_i32, exactly(xs), constant(2)));
        BOOST_TEST(is_gep_of(gep_tmp_0, is_i32, exactly(alloca), constant(0)));
        BOOST_TEST(is_gep_of(gep_tmp_1, is_i32, exactly(alloca), constant(2)));
        BOOST_TEST(is_bitcast_of(dst_tmp_0, exactly(gep_tmp_0), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(dst_tmp_1, exactly(gep_tmp_1), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(src_xs_0, exactly(gep_xs_0), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(src_xs_1, exactly(gep_xs_1), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(
            is_direct_call(
                memcpy_0,
                exactly(memcpy_fn),
                call_arg(exactly(dst_tmp_0), {llvm::Attribute::Alignment}, llvm::Align(4)),
                call_arg(exactly(src_xs_1), {llvm::Attribute::Alignment}, llvm::Align(4)),
                call_arg(constant(8)),
                call_arg(constant(false))));
        BOOST_TEST(
            is_direct_call(
                memcpy_1,
                exactly(memcpy_fn),
                call_arg(exactly(dst_tmp_1), {llvm::Attribute::Alignment}, llvm::Align(4)),
                call_arg(exactly(src_xs_0), {llvm::Attribute::Alignment}, llvm::Align(4)),
                call_arg(constant(8)),
                call_arg(constant(false))));
        BOOST_TEST(is_direct_call(call, exactly(f), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call)));
    }
    {
        auto const f = pass_result.value()->getFunction("h");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 13ul);
        auto const alloca      = inst[0];
        auto const gep_tmp_0   = inst[1];
        auto const gep_xs_1    = inst[2];
        auto const dst_tmp_0   = inst[3];
        auto const src_xs_1    = inst[4];
        auto const memcpy_0    = inst[5];
        auto const gep_tmp_1   = inst[6];
        auto const gep_xs_0    = inst[7];
        auto const dst_tmp_1   = inst[8];
        auto const src_xs_0    = inst[9];
        auto const memcpy_1    = inst[10];
        auto const call        = inst[11];
        auto const ret         = inst[12];
        auto const xs = f->getArg(0ul);
        BOOST_TEST(is_alloca(alloca, is_i32, constant(8), align_of(4)));
        BOOST_TEST(is_gep_of(gep_xs_0, is_i32, exactly(xs), constant(0)));
        BOOST_TEST(is_gep_of(gep_xs_1, is_i32, exactly(xs), constant(4)));
        BOOST_TEST(is_gep_of(gep_tmp_0, is_i32, exactly(alloca), constant(0)));
        BOOST_TEST(is_gep_of(gep_tmp_1, is_i32, exactly(alloca), constant(4)));
        BOOST_TEST(is_bitcast_of(dst_tmp_0, exactly(gep_tmp_0), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(dst_tmp_1, exactly(gep_tmp_1), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(src_xs_0, exactly(gep_xs_0), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(is_bitcast_of(src_xs_1, exactly(gep_xs_1), pointer_to(is_i32), pointer_to(is_i8)));
        BOOST_TEST(
            is_direct_call(
                memcpy_0,
                exactly(memcpy_fn),
                call_arg(exactly(dst_tmp_0), {llvm::Attribute::Alignment}, llvm::Align(4)),
                call_arg(exactly(src_xs_1), {llvm::Attribute::Alignment}, llvm::Align(4)),
                call_arg(constant(16)),
                call_arg(constant(false))));
        BOOST_TEST(
            is_direct_call(
                memcpy_1,
                exactly(memcpy_fn),
                call_arg(exactly(dst_tmp_1), {llvm::Attribute::Alignment}, llvm::Align(4)),
                call_arg(exactly(src_xs_0), {llvm::Attribute::Alignment}, llvm::Align(4)),
                call_arg(constant(16)),
                call_arg(constant(false))));
        BOOST_TEST(is_direct_call(call, exactly(f), call_arg(exactly(alloca))));
        BOOST_TEST(is_return_of(ret, exactly(call)));
    }
}

BOOST_AUTO_TEST_CASE(pass_018)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_018.depc"));
    {
        auto const f = pass_result.value()->getFunction("test");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "which", zext)}, is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 7ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        auto const cont0 = blks[3];
        auto const then1 = blks[4];
        auto const else2 = blks[5];
        auto const cont3 = blks[6];
        auto const fn_ptr = fnptr_type(std::tuple{pointer_to(is_i32)}, is_i32);
        auto const trace = exactly(pass_result.value()->getFunction("trace"));
        auto const anti_trace = exactly(pass_result.value()->getFunction("anti_trace"));
        auto const tmp_f = [&]
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_alloca(inst[0], fn_ptr, constant(1), align_of(8)));
            BOOST_TEST(is_branch_of(inst[1], exactly(f->getArg(0)), exactly(then0), exactly(else0)));
            return inst[0];
        }();
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], fn_ptr, trace, exactly(tmp_f), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont0)));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_store_of(inst[0], fn_ptr, anti_trace, exactly(tmp_f), align_of(8)));
            BOOST_TEST(is_unconditional_branch_to(inst[1], exactly(cont0)));
        }
        auto const [load_f, alloca] = [&]
        {
            auto const inst = get_instructions(*cont0);
            BOOST_TEST_REQUIRE(inst.size() == 3ul);
            BOOST_TEST(is_load_of(inst[0], fn_ptr, exactly(tmp_f), align_of(8)));
            BOOST_TEST(is_alloca(inst[1], is_i32, constant(4), align_of(4)));
            BOOST_TEST(is_branch_of(inst[2], exactly(f->getArg(0)), exactly(then1), exactly(else2)));
            return std::tuple{inst[0], inst[1]};
        }();
        {
            auto const inst = get_instructions(*then1);
            BOOST_TEST_REQUIRE(inst.size() == 11ul);
            auto const gep_row_0    = inst[0];
            auto const gep_val_0_0  = inst[1];
            auto const store_0_0    = inst[2];
            auto const gep_val_0_1  = inst[3];
            auto const store_0_1    = inst[4];
            auto const gep_row_1    = inst[5];
            auto const gep_val_1_0  = inst[6];
            auto const store_1_0    = inst[7];
            auto const gep_val_1_1  = inst[8];
            auto const store_1_1    = inst[9];
            auto const br           = inst[10];
            auto const ret_arg = f->getArg(0ul);
            BOOST_TEST(is_gep_of(gep_row_0, is_i32, exactly(alloca), constant(0)));
            BOOST_TEST(is_gep_of(gep_row_1, is_i32, exactly(alloca), constant(2)));
            BOOST_TEST(is_gep_of(gep_val_0_0, is_i32, exactly(gep_row_0), constant(0)));
            BOOST_TEST(is_gep_of(gep_val_0_1, is_i32, exactly(gep_row_0), constant(1)));
            BOOST_TEST(is_gep_of(gep_val_1_0, is_i32, exactly(gep_row_1), constant(0)));
            BOOST_TEST(is_gep_of(gep_val_1_1, is_i32, exactly(gep_row_1), constant(1)));
            BOOST_TEST(is_store_of(store_0_0, is_i32, constant(1), exactly(gep_val_0_0), align_of(4)));
            BOOST_TEST(is_store_of(store_0_1, is_i32, constant(0), exactly(gep_val_0_1), align_of(4)));
            BOOST_TEST(is_store_of(store_1_0, is_i32, constant(0), exactly(gep_val_1_0), align_of(4)));
            BOOST_TEST(is_store_of(store_1_1, is_i32, constant(1), exactly(gep_val_1_1), align_of(4)));
            BOOST_TEST(is_unconditional_branch_to(br, exactly(cont3)));
        }
        {
            auto const inst = get_instructions(*else2);
            BOOST_TEST_REQUIRE(inst.size() == 11ul);
            auto const gep_row_0    = inst[0];
            auto const gep_val_0_0  = inst[1];
            auto const store_0_0    = inst[2];
            auto const gep_val_0_1  = inst[3];
            auto const store_0_1    = inst[4];
            auto const gep_row_1    = inst[5];
            auto const gep_val_1_0  = inst[6];
            auto const store_1_0    = inst[7];
            auto const gep_val_1_1  = inst[8];
            auto const store_1_1    = inst[9];
            auto const br           = inst[10];
            auto const ret_arg = f->getArg(0ul);
            BOOST_TEST(is_gep_of(gep_row_0, is_i32, exactly(alloca), constant(0)));
            BOOST_TEST(is_gep_of(gep_row_1, is_i32, exactly(alloca), constant(2)));
            BOOST_TEST(is_gep_of(gep_val_0_0, is_i32, exactly(gep_row_0), constant(0)));
            BOOST_TEST(is_gep_of(gep_val_0_1, is_i32, exactly(gep_row_0), constant(1)));
            BOOST_TEST(is_gep_of(gep_val_1_0, is_i32, exactly(gep_row_1), constant(0)));
            BOOST_TEST(is_gep_of(gep_val_1_1, is_i32, exactly(gep_row_1), constant(1)));
            BOOST_TEST(is_store_of(store_0_0, is_i32, constant(0), exactly(gep_val_0_0), align_of(4)));
            BOOST_TEST(is_store_of(store_0_1, is_i32, constant(1), exactly(gep_val_0_1), align_of(4)));
            BOOST_TEST(is_store_of(store_1_0, is_i32, constant(1), exactly(gep_val_1_0), align_of(4)));
            BOOST_TEST(is_store_of(store_1_1, is_i32, constant(0), exactly(gep_val_1_1), align_of(4)));
            BOOST_TEST(is_unconditional_branch_to(br, exactly(cont3)));
        }
        {
            auto const inst = get_instructions(*cont3);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_indirect_call(inst[0], exactly(load_f), call_arg(exactly(alloca))));
            BOOST_TEST(is_return_of(inst[1], exactly(inst[0])));
        }
    }
}

BOOST_AUTO_TEST_CASE(pass_019)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_019.depc"));
    {
        auto const f = pass_result.value()->getFunction("get");
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
        {
            auto const inst = get_instructions(*entry);
            BOOST_TEST_REQUIRE(inst.size() == 2ul);
            BOOST_TEST(is_cmp(inst[0], llvm::CmpInst::Predicate::ICMP_ULT, exactly(i), constant(3)));
            BOOST_TEST(is_branch_of(inst[1], exactly(inst[0]), exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 3ul);
            BOOST_TEST(is_gep_of(inst[0], is_i32, exactly(xs), exactly(i)));
            BOOST_TEST(is_load_of(inst[1], is_i32, exactly(inst[0]), align_of(4)));
            BOOST_TEST(is_return_of(inst[2], exactly(inst[1])));
        }
        {
            BOOST_TEST(is_return_of(else0->getTerminator(), constant(0)));
        }
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)
// BOOST_AUTO_TEST_CASE(typecheck_error_003)

BOOST_AUTO_TEST_SUITE_END()
