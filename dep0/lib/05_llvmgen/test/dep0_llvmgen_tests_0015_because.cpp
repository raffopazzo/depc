#define BOOST_TEST_MODULE dep0_llvmgen_tests_0015_because
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0015_because, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0015_because/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i64, "n", zext), arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(1ul)), constant(0)), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
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
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(2ul)), exactly(f->getArg(0ul))), align_of(4))));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i64, "n", zext), arg_of(pointer_to(is_i32), "xs", nonnull)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                load_of(is_i32, gep_of(is_i32, exactly(f->getArg(1ul)), constant(0)), align_of(4))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0015_because/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0ul))));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
}

BOOST_AUTO_TEST_SUITE_END()
