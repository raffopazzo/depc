#define BOOST_TEST_MODULE dep0_llvmgen_tests_0019_div_expr
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0019_div_expr, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("two");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("minus_two");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("two");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(2)));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", sext), arg_of(is_i32, "b", sext), arg_of(is_i32, "c", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(sdiv_of(a, b), c)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", sext), arg_of(is_i32, "b", sext), arg_of(is_i32, "c", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(a, sdiv_of(b, c))));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", sext), arg_of(is_i32, "b", sext), arg_of(is_i32, "c", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), sub_of(a, sdiv_of(b, c))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", sext), arg_of(is_i32, "b", sext), arg_of(is_i32, "c", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), sub_of(sdiv_of(a, b), c)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", sext), arg_of(is_i32, "b", sext), arg_of(is_i32, "c", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), mul_of(sdiv_of(a, b), c)));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", sext), arg_of(is_i32, "b", sext), arg_of(is_i32, "c", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), sdiv_of(mul_of(a, b), c)));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0019_div_expr/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", zext), arg_of(is_i32, "b", zext), arg_of(is_i32, "c", zext)},
                is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(udiv_of(a, b), c)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", zext), arg_of(is_i32, "b", zext), arg_of(is_i32, "c", zext)},
                is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(a, udiv_of(b, c))));
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", zext), arg_of(is_i32, "b", zext), arg_of(is_i32, "c", zext)},
                is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), sub_of(a, udiv_of(b, c))));
    }
    {
        auto const f = pass_result.value()->getFunction("f4");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", zext), arg_of(is_i32, "b", zext), arg_of(is_i32, "c", zext)},
                is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), sub_of(udiv_of(a, b), c)));
    }
    {
        auto const f = pass_result.value()->getFunction("f5");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", zext), arg_of(is_i32, "b", zext), arg_of(is_i32, "c", zext)},
                is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), mul_of(udiv_of(a, b), c)));
    }
    {
        auto const f = pass_result.value()->getFunction("f6");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "a", zext), arg_of(is_i32, "b", zext), arg_of(is_i32, "c", zext)},
                is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const a = exactly(f->getArg(0ul));
        auto const b = exactly(f->getArg(1ul));
        auto const c = exactly(f->getArg(2ul));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), udiv_of(mul_of(a, b), c)));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)

BOOST_AUTO_TEST_SUITE_END()
