#define BOOST_TEST_MODULE dep0_llvmgen_tests_0009_func_decl
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;
using enum llvm::CmpInst::Predicate;

static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0009_func_decl, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST(f->size() == 0ul);
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, std::nullopt, sext)}, is_i1, zext));
        BOOST_TEST(f->size() == 0ul);
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST(f->size() == 0ul);
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(exactly(pass_result.value()->getFunction("f")))));
    }
}

BOOST_AUTO_TEST_SUITE_END()
