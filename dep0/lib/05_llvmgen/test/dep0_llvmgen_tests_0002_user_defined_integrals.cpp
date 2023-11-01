#define BOOST_TEST_MODULE dep0_llvmgen_tests_0002_user_defined_integrals
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0002_user_defined_integrals, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(test_0152)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/test_0152.depc"));
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

// BOOST_AUTO_TEST_CASE(test_0153) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0154) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0155) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0156) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0157) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0158) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0159) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0160) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0161) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0162) doesn't parse
BOOST_AUTO_TEST_CASE(test_0163)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/test_0163.depc"));
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

BOOST_AUTO_TEST_CASE(test_0164)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/test_0164.depc"));
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
// BOOST_AUTO_TEST_CASE(test_0165) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0166) doesn't parse

BOOST_AUTO_TEST_SUITE_END()
