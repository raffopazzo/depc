#define BOOST_TEST_MODULE dep0_llvmgen_tests_0017_minus_expr
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const sext = std::vector{llvm::Attribute::SExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0017_minus_expr, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0017_minus_expr/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("minus_one");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0017_minus_expr/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("minus_one");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0017_minus_expr/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("minus_one");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0017_minus_expr/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("three");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(3)));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0017_minus_expr/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("one");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("two");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(2)));
    }
    {
        auto const f = pass_result.value()->getFunction("minus_one");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("minus_two");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-2)));
    }
    {
        auto const f = pass_result.value()->getFunction("three");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(3)));
    }
    {
        auto const f = pass_result.value()->getFunction("four");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(4)));
    }
    {
        auto const f = pass_result.value()->getFunction("minus_five");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-5)));
    }
    {
        auto const f = pass_result.value()->getFunction("minus_six");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-6)));
    }
    {
        auto const f = pass_result.value()->getFunction("minus_seven");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-7)));
    }
    {
        auto const f = pass_result.value()->getFunction("minus_four");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-4)));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0017_minus_expr/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", sext), arg_of(is_i32, "b", sext), arg_of(is_i32, "c", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(sub_of(a, b), c)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", sext), arg_of(is_i32, "b", sext), arg_of(is_i32, "c", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), sub_of(a, add_of(a, b))));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)

BOOST_AUTO_TEST_SUITE_END()
