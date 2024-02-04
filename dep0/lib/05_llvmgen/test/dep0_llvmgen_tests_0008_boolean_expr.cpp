#define BOOST_TEST_MODULE dep0_llvmgen_tests_0008_boolean_expr
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates_v2.hpp"

using namespace dep0::llvmgen::testing::v2;

template <Predicate<llvm::Value> F>
auto neg_of(F&& f)
{
    return xor_of(std::forward<F>(f), constant(true));
}

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0008_boolean_expr, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_000.depc"));
    auto const slt = llvm::CmpInst::Predicate::ICMP_SLT;
    auto const ult = llvm::CmpInst::Predicate::ICMP_ULT;
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x")}, is_i1));
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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const negate = exactly(pass_result.value()->getFunction("negate"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ult,
                    direct_call_of(negate, call_arg(cmp_of(ult, constant(true), direct_call_of(f1)))),
                    direct_call_of(negate, call_arg(cmp_of(ult, direct_call_of(f1), constant(true)))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", {llvm::Attribute::SExt})}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(slt, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", {llvm::Attribute::ZExt})}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ult, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i8, "x", {llvm::Attribute::SExt}),
                    arg_of(is_i8, "y", {llvm::Attribute::SExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(slt, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", {llvm::Attribute::SExt}),
                    arg_of(is_i32, "y", {llvm::Attribute::SExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(slt, exactly(f->getArg(0ul)), add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", {llvm::Attribute::ZExt}),
                    arg_of(is_i32, "y", {llvm::Attribute::ZExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ult,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i8, "x", {llvm::Attribute::ZExt}),
                    arg_of(is_i8, "y", {llvm::Attribute::ZExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ult,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ult,
                    cmp_of(
                        ult,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ult,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ult,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    slt,
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4))),
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(2)), align_of(4))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_001.depc"));
    auto const sgt = llvm::CmpInst::Predicate::ICMP_SGT;
    auto const ugt = llvm::CmpInst::Predicate::ICMP_UGT;
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x")}, is_i1));
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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const negate = exactly(pass_result.value()->getFunction("negate"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ugt,
                    direct_call_of(negate, call_arg(cmp_of(ugt, constant(true), direct_call_of(f1)))),
                    direct_call_of(negate, call_arg(cmp_of(ugt, direct_call_of(f1), constant(true)))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", {llvm::Attribute::SExt})}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(sgt, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", {llvm::Attribute::ZExt})}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ugt, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i8, "x", {llvm::Attribute::SExt}),
                    arg_of(is_i8, "y", {llvm::Attribute::SExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(sgt, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", {llvm::Attribute::SExt}),
                    arg_of(is_i32, "y", {llvm::Attribute::SExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(sgt, exactly(f->getArg(0ul)), add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", {llvm::Attribute::ZExt}),
                    arg_of(is_i32, "y", {llvm::Attribute::ZExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ugt,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i8, "x", {llvm::Attribute::ZExt}),
                    arg_of(is_i8, "y", {llvm::Attribute::ZExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ugt,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ugt,
                    cmp_of(
                        ugt,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ugt,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ugt,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    sgt,
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4))),
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(2)), align_of(4))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_002.depc"));
    auto const sle = llvm::CmpInst::Predicate::ICMP_SLE;
    auto const ule = llvm::CmpInst::Predicate::ICMP_ULE;
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x")}, is_i1));
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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const negate = exactly(pass_result.value()->getFunction("negate"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ule,
                    direct_call_of(negate, call_arg(cmp_of(ule, constant(true), direct_call_of(f1)))),
                    direct_call_of(negate, call_arg(cmp_of(ule, direct_call_of(f1), constant(true)))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", {llvm::Attribute::SExt})}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(sle, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", {llvm::Attribute::ZExt})}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ule, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i8, "x", {llvm::Attribute::SExt}),
                    arg_of(is_i8, "y", {llvm::Attribute::SExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(sle, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", {llvm::Attribute::SExt}),
                    arg_of(is_i32, "y", {llvm::Attribute::SExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(sle, exactly(f->getArg(0ul)), add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", {llvm::Attribute::ZExt}),
                    arg_of(is_i32, "y", {llvm::Attribute::ZExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ule,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i8, "x", {llvm::Attribute::ZExt}),
                    arg_of(is_i8, "y", {llvm::Attribute::ZExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ule,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ule,
                    cmp_of(
                        ule,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ule,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ule,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    sle,
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4))),
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(2)), align_of(4))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_003.depc"));
    auto const sge = llvm::CmpInst::Predicate::ICMP_SGE;
    auto const uge = llvm::CmpInst::Predicate::ICMP_UGE;
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x")}, is_i1));
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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const negate = exactly(pass_result.value()->getFunction("negate"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    uge,
                    direct_call_of(negate, call_arg(cmp_of(uge, constant(true), direct_call_of(f1)))),
                    direct_call_of(negate, call_arg(cmp_of(uge, direct_call_of(f1), constant(true)))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", {llvm::Attribute::SExt})}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(sge, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", {llvm::Attribute::ZExt})}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(uge, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i8, "x", {llvm::Attribute::SExt}),
                    arg_of(is_i8, "y", {llvm::Attribute::SExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(sge, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", {llvm::Attribute::SExt}),
                    arg_of(is_i32, "y", {llvm::Attribute::SExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(sge, exactly(f->getArg(0ul)), add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i32, "x", {llvm::Attribute::ZExt}),
                    arg_of(is_i32, "y", {llvm::Attribute::ZExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    uge,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i8, "x", {llvm::Attribute::ZExt}),
                    arg_of(is_i8, "y", {llvm::Attribute::ZExt})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    uge,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    uge,
                    cmp_of(
                        uge,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    uge,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        uge,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(pointer_to(is_i32), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    sge,
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4))),
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(2)), align_of(4))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    auto const ult = llvm::CmpInst::Predicate::ICMP_ULT;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), neg_of(exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ult, neg_of(exactly(x)), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), neg_of(cmp_of(ult, exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ult, neg_of(exactly(x)), neg_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ult,
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                neg_of(
                    cmp_of(
                        ult,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ult,
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), and_of(exactly(x), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), neg_of(and_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), and_of(neg_of(exactly(x)), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                neg_of(
                    and_of(
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_006.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), or_of(exactly(x), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), neg_of(or_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), or_of(neg_of(exactly(x)), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    and_of(exactly(x), exactly(y)),
                    and_of(neg_of(exactly(x)), neg_of(exactly(y))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    and_of(exactly(x), or_of(exactly(y), neg_of(exactly(x)))),
                    neg_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    or_of(exactly(x), and_of(exactly(y), neg_of(exactly(x)))),
                    neg_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x"), arg_of(is_i1, "y")}, is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    or_of(exactly(x), and_of(exactly(y), neg_of(exactly(x)))),
                    neg_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f13");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f14");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                neg_of(
                    or_of(
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f15");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(pointer_to(is_i1), "xs", {llvm::Attribute::NonNull})},
                is_i1));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    neg_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)

BOOST_AUTO_TEST_SUITE_END()
