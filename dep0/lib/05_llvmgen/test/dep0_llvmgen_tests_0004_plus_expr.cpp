#define BOOST_TEST_MODULE dep0_llvmgen_tests_0004_plus_expr
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0004_plus_expr, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_000.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_001.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_002.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_003.depc"));
    BOOST_TEST(has_function("minus_one", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(-1)));
        return boost::test_tools::predicate_result(true);
    }));
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)

BOOST_AUTO_TEST_SUITE_END()
