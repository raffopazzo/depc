#define BOOST_TEST_MODULE dep0_llvmgen_tests_0007_arrays
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates_v2.hpp"

using namespace dep0::llvmgen::testing::v2;

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0007_arrays, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("values");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), std::nullopt, {llvm::Attribute::NonNull, llvm::Attribute::StructRet})
                },
                is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() == 7ul);
        auto it = bb.begin();
        auto const& gep1 = *it++;
        auto const& store1 = *it++;
        auto const& gep2 = *it++;
        auto const& store2 = *it++;
        auto const& gep3 = *it++;
        auto const& store3 = *it++;
        auto const& ret = *it++;
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i32, exactly(ret_arg), constant(2)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(&gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(&gep2), align_of(4)));
        BOOST_TEST(is_store_of(store3, is_i32, constant(3), exactly(&gep3), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("empty_of_int");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(
                        pointer_to(is_i32),
                        std::nullopt,
                        {
                            llvm::Attribute::NonNull,
                            llvm::Attribute::StructRet
                        })},
                is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() == 1ul);
        BOOST_TEST(is_return_of_void(bb.front()));
    }
    {
        auto const f = pass_result.value()->getFunction("empty_of_bool");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(
                        pointer_to(is_i1),
                        std::nullopt,
                        {
                            llvm::Attribute::NonNull,
                            llvm::Attribute::StructRet
                        })},
                is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() == 1ul);
        BOOST_TEST(is_return_of_void(bb.front()));
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
                    arg_of(is_i64, "n", {llvm::Attribute::ZExt}),
                    arg_of(pointer_to(is_i32), std::nullopt, {llvm::Attribute::NonNull})},
                is_i8));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() == 1ul);
        BOOST_TEST(is_return_of(bb.front(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("nothing");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8));
        BOOST_TEST(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST(bb.size() == 3ul);
        auto it = bb.begin();
        auto const& alloca = *it++;
        auto const& call = *it++;
        auto const& ret = *it++;
        BOOST_TEST(is_alloca(alloca, is_i32, constant(0), align_of(4)));
        BOOST_TEST(
            is_direct_call(
                call,
                exactly(pass_result.value()->getFunction("discard")),
                call_arg(constant(0), {llvm::Attribute::ZExt}),
                call_arg(exactly(&alloca))));
        BOOST_TEST(is_return_of(ret, constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("values");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), std::nullopt, {llvm::Attribute::NonNull, llvm::Attribute::StructRet})
                },
                is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() == 5ul);
        auto it = bb.begin();
        auto const& gep1 = *it++;
        auto const& store1 = *it++;
        auto const& gep2 = *it++;
        auto const& store2 = *it++;
        auto const& ret = *it++;
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep1, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, exactly(ret_arg), constant(1)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(1), exactly(&gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(2), exactly(&gep2), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("first");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f, std::tuple{arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})}, is_i32));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(0ul)), constant(0)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("second");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})}, is_i32));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(0ul)), constant(1)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("third");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})}, is_i32));
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
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})}, is_i32));
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
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() >= 7ul);
        auto it = bb.begin();
        auto const alloca = exactly(&*it++);
        auto const& gep1 = *it++;
        auto const& store1 = *it++;
        auto const& gep2 = *it++;
        auto const& store2 = *it++;
        auto const& gep3 = *it++;
        auto const& store3 = *it++;
        BOOST_TEST(is_gep_of(gep1, is_i32, alloca, constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i32, alloca, constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i32, alloca, constant(2)));
        BOOST_TEST(is_store_of(store1, is_i32, constant(3), exactly(&gep1), align_of(4)));
        BOOST_TEST(is_store_of(store2, is_i32, constant(5), exactly(&gep2), align_of(4)));
        BOOST_TEST(is_store_of(store3, is_i32, constant(7), exactly(&gep3), align_of(4)));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("sum")),
                    call_arg(alloca))));
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
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i8), std::nullopt, {llvm::Attribute::NonNull, llvm::Attribute::StructRet})
                },
                is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() == 10ul);
        auto it = bb.begin();
        auto const& gep1 = *it++;
        auto const& call1 = *it++;
        auto const& store1 = *it++;
        auto const& gep2 = *it++;
        auto const& call2 = *it++;
        auto const& store2 = *it++;
        auto const& gep3 = *it++;
        auto const& call3 = *it++;
        auto const& store3 = *it++;
        auto const& ret = *it++;
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep1, is_i8, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep2, is_i8, exactly(ret_arg), constant(1)));
        BOOST_TEST(is_gep_of(gep3, is_i8, exactly(ret_arg), constant(2)));
        auto const unit = pass_result.value()->getFunction("unit");
        BOOST_TEST(is_direct_call(call1, exactly(unit)));
        BOOST_TEST(is_direct_call(call2, exactly(unit)));
        BOOST_TEST(is_direct_call(call3, exactly(unit)));
        BOOST_TEST(is_store_of(store1, is_i8, exactly(&call1), exactly(&gep1), align_of(1)));
        BOOST_TEST(is_store_of(store2, is_i8, exactly(&call2), exactly(&gep2), align_of(1)));
        BOOST_TEST(is_store_of(store3, is_i8, exactly(&call3), exactly(&gep3), align_of(1)));
        BOOST_TEST(is_return_of_void(ret));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_008.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), std::nullopt, {llvm::Attribute::NonNull, llvm::Attribute::StructRet})
                },
                is_void));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() == 15ul);
        auto it = bb.begin();
        auto const& gep_row_0 = *it++;
        auto const& gep_val_0_0 = *it++;
        auto const& store_0_0 = *it++;
        auto const& gep_val_0_1 = *it++;
        auto const& store_0_1 = *it++;
        auto const& gep_val_0_2 = *it++;
        auto const& store_0_2 = *it++;
        auto const& gep_row_1 = *it++;
        auto const& gep_val_1_0 = *it++;
        auto const& store_1_0 = *it++;
        auto const& gep_val_1_1 = *it++;
        auto const& store_1_1 = *it++;
        auto const& gep_val_1_2 = *it++;
        auto const& store_1_2 = *it++;
        auto const& ret = *it++;
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST(is_gep_of(gep_row_0, is_i32, exactly(ret_arg), constant(0)));
        BOOST_TEST(is_gep_of(gep_row_1, is_i32, exactly(ret_arg), constant(3)));
        BOOST_TEST(is_gep_of(gep_val_0_0, is_i32, exactly(&gep_row_0), constant(0)));
        BOOST_TEST(is_gep_of(gep_val_0_1, is_i32, exactly(&gep_row_0), constant(1)));
        BOOST_TEST(is_gep_of(gep_val_0_2, is_i32, exactly(&gep_row_0), constant(2)));
        BOOST_TEST(is_gep_of(gep_val_1_0, is_i32, exactly(&gep_row_1), constant(0)));
        BOOST_TEST(is_gep_of(gep_val_1_1, is_i32, exactly(&gep_row_1), constant(1)));
        BOOST_TEST(is_gep_of(gep_val_1_2, is_i32, exactly(&gep_row_1), constant(2)));
        BOOST_TEST(is_store_of(store_0_0, is_i32, constant(1), exactly(&gep_val_0_0), align_of(4)));
        BOOST_TEST(is_store_of(store_0_1, is_i32, constant(2), exactly(&gep_val_0_1), align_of(4)));
        BOOST_TEST(is_store_of(store_0_2, is_i32, constant(3), exactly(&gep_val_0_2), align_of(4)));
        BOOST_TEST(is_store_of(store_1_0, is_i32, constant(4), exactly(&gep_val_1_0), align_of(4)));
        BOOST_TEST(is_store_of(store_1_1, is_i32, constant(5), exactly(&gep_val_1_1), align_of(4)));
        BOOST_TEST(is_store_of(store_1_2, is_i32, constant(6), exactly(&gep_val_1_2), align_of(4)));
        BOOST_TEST(is_return_of_void(ret));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f, std::tuple{arg_of(pointer_to(is_i32), "m", {llvm::Attribute::NonNull})}, is_i32));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        BOOST_TEST_REQUIRE(bb.size() >= 6ul);
        auto it = bb.begin();
        auto const& gep_row_0 = *it++;
        auto const& gep_val_0_1 = *it++;
        auto const& load_val_0_1 = *it++;
        auto const& gep_row_1 = *it++;
        auto const& gep_val_1_2 = *it++;
        auto const& load_val_1_2 = *it++;
        BOOST_TEST(is_return_of(bb.getTerminator(), add_of(exactly(&load_val_0_1), exactly(&load_val_1_2))));
    }
}

BOOST_AUTO_TEST_CASE(pass_009)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_009.depc"));
    {
        auto const f = pass_result.value()->getFunction("sum");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})},
                is_i32));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = exactly(f->getArg(0ul));
        auto const& bb = f->getEntryBlock();
        BOOST_TEST(
            is_return_of(
                bb.getTerminator(),
                add_of(
                    add_of(
                        load_of(is_i32, gep_of(is_i32, xs, constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, xs, constant(1)), align_of(4))),
                    load_of(is_i32, gep_of(is_i32, xs, constant(2)), align_of(4)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i32), "m", {llvm::Attribute::NonNull})},
                is_i32));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const& bb = f->getEntryBlock();
        auto const m = exactly(f->getArg(0ul));
        BOOST_TEST(
            is_return_of(
                bb.getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("sum")),
                    call_arg(gep_of(is_i32, m, constant(6))))));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i32), "m", {llvm::Attribute::NonNull})},
                is_i32));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const m = exactly(f->getArg(0ul));
        auto const sum = exactly(pass_result.value()->getFunction("sum"));
        auto const& bb = f->getEntryBlock();
        BOOST_TEST(
            is_return_of(
                bb.getTerminator(),
                add_of(
                    add_of(
                        add_of(
                            direct_call_of(sum, call_arg(gep_of(is_i32, m, constant(0)))),
                            direct_call_of(sum, call_arg(gep_of(is_i32, m, constant(3))))),
                        direct_call_of(sum, call_arg(gep_of(is_i32, m, constant(6))))),
                    direct_call_of(sum, call_arg(gep_of(is_i32, m, constant(9)))))));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)

BOOST_AUTO_TEST_SUITE_END()
