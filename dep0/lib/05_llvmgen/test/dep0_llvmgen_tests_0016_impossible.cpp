#define BOOST_TEST_MODULE dep0_llvmgen_tests_0016_impossible
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

// static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0016_impossible, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_unreachable(f->getEntryBlock().getTerminator()));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("a_lt_b_implies_b");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i1, "a", zext), arg_of(is_i1, "b", zext)},
                struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), is_zeroinitializer));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("a_gt_b_implies_a");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i1, "a", zext), arg_of(is_i1, "b", zext)},
                struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), is_zeroinitializer));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "a", zext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "a", zext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("a_lt_b_implies_b");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{arg_of(is_i1, "a", zext), arg_of(is_i1, "b", zext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{arg_of(is_i1, "a", zext), arg_of(is_i1, "b", zext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("a_lt_b_implies_b2");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{arg_of(is_i1, "a", zext), arg_of(is_i1, "b", zext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{arg_of(is_i1, "a", zext), arg_of(is_i1, "b", zext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(f,
                std::tuple{arg_of(is_i1, "a", zext), arg_of(is_i1, "b", zext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
}

BOOST_AUTO_TEST_SUITE_END()
