#define BOOST_TEST_MODULE dep0_llvmgen_tests_0012_auto_expr
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0012_auto_expr, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i64, "i", zext),
                    arg_of(is_i64, "n", zext),
                    arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(2)), exactly(f->getArg(0))), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i64, "n", zext),
                    arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        BOOST_TEST(
            is_branch_of(
                entry->getTerminator(),
                cmp_of(llvm::CmpInst::Predicate::ICMP_ULT, constant(0), exactly(f->getArg(0ul))),
                exactly(then0),
                exactly(else0)));
        BOOST_TEST(
            is_return_of(
                then0->getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("f")),
                    call_arg(constant(0), zext),
                    call_arg(exactly(f->getArg(0ul)), zext),
                    call_arg(exactly(f->getArg(1ul))))));
        BOOST_TEST(is_return_of(else0->getTerminator(), constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i64, "i", zext),
                    arg_of(is_i64, "n", zext),
                    arg_of(struct_of()),
                    arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(3)), exactly(f->getArg(0))), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    arg_of(is_i64, "n", zext),
                    arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        BOOST_TEST(
            is_branch_of(
                entry->getTerminator(),
                cmp_of(llvm::CmpInst::Predicate::ICMP_ULT, constant(0), exactly(f->getArg(0ul))),
                exactly(then0),
                exactly(else0)));
        BOOST_TEST(
            is_return_of(
                then0->getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("f")),
                    call_arg(constant(0), zext),
                    call_arg(exactly(f->getArg(0ul)), zext),
                    call_arg(is_zeroinitializer),
                    call_arg(exactly(f->getArg(1ul))))));
        BOOST_TEST(is_return_of(else0->getTerminator(), constant(0)));
    }
}

BOOST_AUTO_TEST_SUITE_END()
