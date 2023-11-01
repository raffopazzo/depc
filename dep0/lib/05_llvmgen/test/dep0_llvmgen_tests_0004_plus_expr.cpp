#define BOOST_TEST_MODULE dep0_llvmgen_tests_0004_plus_expr
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0004_plus_expr, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(test_0173)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/test_0173.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0174)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/test_0174.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0175)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/test_0175.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0176)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/test_0176.depc"));
    BOOST_TEST(has_function("minus_one", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(-1)));
        return boost::test_tools::predicate_result(true);
    }));
}

// BOOST_AUTO_TEST_CASE(test_0177) doesn't type check

BOOST_AUTO_TEST_SUITE_END()
