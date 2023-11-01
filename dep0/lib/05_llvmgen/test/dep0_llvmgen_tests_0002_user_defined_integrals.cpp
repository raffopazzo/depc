#define BOOST_TEST_MODULE dep0_llvmgen_tests_0002_user_defined_integrals
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0002_user_defined_integrals, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_000.depc"));
    BOOST_TEST(has_function("h", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), constant_int([] (llvm::ConstantInt const& c)
        {
            BOOST_TEST(c.getType()->isIntegerTy(8));
            BOOST_TEST(c.getZExtValue() == 23ul);
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("n", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), constant_int([] (llvm::ConstantInt const& c)
        {
            BOOST_TEST(c.getType()->isIntegerTy(32));
            BOOST_TEST(c.getZExtValue() == 123456789ul);
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("d", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), constant_int([] (llvm::ConstantInt const& c)
        {
            BOOST_TEST(c.getType()->isIntegerTy(64));
            BOOST_TEST(c.getSExtValue() == -1234567890123456789l);
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_001.depc"));
    BOOST_TEST(has_function("zero", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), constant_int([] (llvm::ConstantInt const& c)
        {
            BOOST_TEST(c.getType()->isIntegerTy(8));
            BOOST_TEST(c.getSExtValue() == 0);
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_002.depc"));
    BOOST_TEST(has_function("zero", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), constant_int([] (llvm::ConstantInt const& c)
        {
            BOOST_TEST(c.getType()->isIntegerTy(8));
            BOOST_TEST(c.getZExtValue() == 0);
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
}

// BOOST_AUTO_TEST_CASE(parse_error_000)
// BOOST_AUTO_TEST_CASE(parse_error_001)
// BOOST_AUTO_TEST_CASE(parse_error_002)
// BOOST_AUTO_TEST_CASE(parse_error_003)
// BOOST_AUTO_TEST_CASE(parse_error_004)
// BOOST_AUTO_TEST_CASE(parse_error_005)
// BOOST_AUTO_TEST_CASE(parse_error_006)
// BOOST_AUTO_TEST_CASE(parse_error_007)
// BOOST_AUTO_TEST_CASE(parse_error_008)
// BOOST_AUTO_TEST_CASE(parse_error_009)
// BOOST_AUTO_TEST_CASE(parse_error_010)

// BOOST_AUTO_TEST_CASE(typecheck_error_000)

BOOST_AUTO_TEST_SUITE_END()
