#define BOOST_TEST_MODULE dep0_llvmgen_tests_0006_kinds
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0006_kinds, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_000.depc"));
    BOOST_TEST(has_function("zero", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_001.depc"));
    BOOST_TEST(has_function("zero", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_002.depc"));
    BOOST_TEST(has_function("zero", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_SUITE_END()
