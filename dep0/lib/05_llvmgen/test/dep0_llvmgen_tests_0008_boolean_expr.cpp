#define BOOST_TEST_MODULE dep0_llvmgen_tests_0008_boolean_expr
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;
using enum llvm::CmpInst::Predicate;

template <Predicate<llvm::Value> F>
auto not_of(F&& f)
{
    return xor_of(std::forward<F>(f), constant(true));
}

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0008_boolean_expr, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_000.depc"));
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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const negate = exactly(pass_result.value()->getFunction("negate"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULT,
                    direct_call_of(negate, call_arg(cmp_of(ICMP_ULT, constant(true), direct_call_of(f1)), zext)),
                    direct_call_of(negate, call_arg(cmp_of(ICMP_ULT, direct_call_of(f1), constant(true)), zext)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_SLT, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_ULT, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i8, "x", sext), arg_of(is_i8, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_SLT, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext), arg_of(is_i32, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_SLT, exactly(f->getArg(0ul)), add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext), arg_of(is_i32, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULT,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i8, "x", zext), arg_of(is_i8, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULT,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULT,
                    cmp_of(
                        ICMP_ULT,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULT,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ICMP_ULT,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_SLT,
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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const negate = exactly(pass_result.value()->getFunction("negate"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGT,
                    direct_call_of(negate, call_arg(cmp_of(ICMP_UGT, constant(true), direct_call_of(f1)), zext)),
                    direct_call_of(negate, call_arg(cmp_of(ICMP_UGT, direct_call_of(f1), constant(true)), zext)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_SGT, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_UGT, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i8, "x", sext), arg_of(is_i8, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_SGT, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext), arg_of(is_i32, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_SGT, exactly(f->getArg(0ul)), add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext), arg_of(is_i32, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGT,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i8, "x", zext), arg_of(is_i8, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGT,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGT,
                    cmp_of(
                        ICMP_UGT,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGT,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ICMP_UGT,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_SGT,
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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const negate = exactly(pass_result.value()->getFunction("negate"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULE,
                    direct_call_of(negate, call_arg(cmp_of(ICMP_ULE, constant(true), direct_call_of(f1)), zext)),
                    direct_call_of(negate, call_arg(cmp_of(ICMP_ULE, direct_call_of(f1), constant(true)), zext)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_SLE, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_ULE, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i8, "x", sext), arg_of(is_i8, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_SLE, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext), arg_of(is_i32, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_SLE, exactly(f->getArg(0ul)), add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext), arg_of(is_i32, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULE,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i8, "x", zext), arg_of(is_i8, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULE,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULE,
                    cmp_of(
                        ICMP_ULE,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULE,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ICMP_ULE,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_SLE,
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
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const negate = exactly(pass_result.value()->getFunction("negate"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGE,
                    direct_call_of(negate, call_arg(cmp_of(ICMP_UGE, constant(true), direct_call_of(f1)), zext)),
                    direct_call_of(negate, call_arg(cmp_of(ICMP_UGE, direct_call_of(f1), constant(true)), zext)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_SGE, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_UGE, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i8, "x", sext), arg_of(is_i8, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_SGE, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext), arg_of(is_i32, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_SGE, exactly(f->getArg(0ul)), add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext), arg_of(is_i32, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGE,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i8, "x", zext), arg_of(is_i8, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGE,
                    add_of(exactly(f->getArg(0ul)), exactly(f->getArg(1ul))),
                    exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGE,
                    cmp_of(
                        ICMP_UGE,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_UGE,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ICMP_UGE,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_SGE,
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
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), not_of(exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), cmp_of(ICMP_ULT, not_of(exactly(x)), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), not_of(cmp_of(ICMP_ULT, exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_ULT, not_of(exactly(x)), not_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULT,
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                not_of(
                    cmp_of(
                        ICMP_ULT,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_ULT,
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), and_of(exactly(x), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), not_of(and_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), and_of(not_of(exactly(x)), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
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
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                not_of(
                    and_of(
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_006.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), or_of(exactly(x), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), not_of(or_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), or_of(not_of(exactly(x)), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    and_of(exactly(x), exactly(y)),
                    and_of(not_of(exactly(x)), not_of(exactly(y))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    and_of(exactly(x), or_of(exactly(y), not_of(exactly(x)))),
                    not_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    or_of(exactly(x), and_of(exactly(y), not_of(exactly(x)))),
                    not_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    or_of(exactly(x), and_of(exactly(y), not_of(exactly(x)))),
                    not_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
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
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f14");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                not_of(
                    or_of(
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f15");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_007.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), xor_of(exactly(x), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), not_of(xor_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), xor_of(not_of(exactly(x)), exactly(y))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                or_of(
                    and_of(exactly(x), exactly(y)),
                    xor_of(not_of(exactly(x)), not_of(exactly(y))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    exactly(x),
                    xor_of(
                        or_of(exactly(y), not_of(exactly(x))),
                        not_of(exactly(y))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                and_of(
                    xor_of(exactly(x), exactly(y)),
                    xor_of(not_of(exactly(x)), not_of(exactly(y))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "x", zext), arg_of(is_i1, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                xor_of(
                    xor_of(exactly(x), and_of(exactly(y), not_of(exactly(x)))),
                    not_of(exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                xor_of(
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f13");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                xor_of(
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f14");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                not_of(
                    xor_of(
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f15");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                xor_of(
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1))),
                    not_of(load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_008.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_EQ, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_EQ, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(is_i8, "x", sext), arg_of(is_i8, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_EQ, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(is_i32, "x", sext), arg_of(is_i32, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_EQ, exactly(x), add_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(is_i32, "x", zext), arg_of(is_i32, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_EQ,
                    add_of(exactly(x), exactly(y)),
                    add_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(is_i8, "x", zext), arg_of(is_i8, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_EQ,
                    add_of(exactly(x), exactly(y)),
                    exactly(x))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_EQ,
                    cmp_of(
                        ICMP_EQ,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_EQ,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ICMP_EQ,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f13");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_EQ,
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4))),
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(2)), align_of(4))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_009)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_009.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(true)));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(false)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_NE, constant(1), exactly(f->getArg(0ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_NE, exactly(f->getArg(0ul)), constant(2))));
    }
    {
        auto const f = pass_result.value()->getFunction("f7");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(is_i8, "x", sext), arg_of(is_i8, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_NE, exactly(f->getArg(0ul)), exactly(f->getArg(1ul)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f8");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(is_i32, "x", sext), arg_of(is_i32, "y", sext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(ICMP_NE, exactly(x), add_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f9");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(is_i32, "x", zext), arg_of(is_i32, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_NE,
                    add_of(exactly(x), exactly(y)),
                    add_of(exactly(x), exactly(y)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f10");
        BOOST_TEST_REQUIRE(
            is_function_of(f, std::tuple{arg_of(is_i8, "x", zext), arg_of(is_i8, "y", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = f->getArg(0ul);
        auto const y = f->getArg(1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_NE,
                    add_of(exactly(x), exactly(y)),
                    exactly(x))));
    }
    {
        auto const f = pass_result.value()->getFunction("f11");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_EQ,
                    cmp_of(
                        ICMP_NE,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1))),
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1)))));
    }
    {
        auto const f = pass_result.value()->getFunction("f12");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i1), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_NE,
                    load_of(is_i1, gep_of(is_i1, exactly(xs), constant(0)), align_of(1)),
                    cmp_of(
                        ICMP_EQ,
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(1)), align_of(1)),
                        load_of(is_i1, gep_of(is_i1, exactly(xs), constant(2)), align_of(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f13");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = f->getArg(0ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    ICMP_NE,
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(0)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4))),
                    add_of(
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(1)), align_of(4)),
                        load_of(is_i32, gep_of(is_i32, exactly(xs), constant(2)), align_of(4))))));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)

BOOST_AUTO_TEST_SUITE_END()
