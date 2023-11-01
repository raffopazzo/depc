#define BOOST_TEST_MODULE dep0_llvmgen_tests_0001_integral_primitives
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0001_integral_primitives, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(test_0024)
{
    BOOST_TEST_REQUIRE(pass("0001_integral_primitives/test_0024.depc"));
    BOOST_TEST(has_function("f0", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(127)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f18", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(32767)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f42", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(2147483647)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f81", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(9223372036854775807)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f141", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(255ul)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f153", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(65535ul)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f169", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(4294967295ul)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f195", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(18446744073709551615ul)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f235", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(9999999999999999999ul)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f237", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(9999999999999999999ul)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f238", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(9999999999999999999ul)));
        return boost::test_tools::predicate_result(true);
    }));
}

// BOOST_AUTO_TEST_CASE(test_0025) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0025) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0026) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0027) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0028) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0029) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0030) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0031) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0032) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0033) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0034) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0035) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0036) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0037) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0038) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0039) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0040) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0041) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0042) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0043) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0044) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0045) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0046) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0047) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0048) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0049) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0050) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0051) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0052) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0053) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0054) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0055) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0056) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0057) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0058) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0059) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0060) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0061) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0062) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0063) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0064) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0065) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0066) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0067) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0068) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0069) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0070) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0071) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0072) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0073) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0074) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0075) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0076) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0077) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0078) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0079) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0080) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0081) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0082) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0083) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0084) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0085) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0086) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0087) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0088) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0089) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0090) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0091) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0092) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0093) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0094) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0095) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0096) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0097) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0098) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0099) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0100) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0101) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0102) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0103) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0104) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0105) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0106) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0107) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0108) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0109) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0110) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0111) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0112) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0113) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0114) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0115) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0116) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0117) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0118) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0119) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0120) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0121) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0122) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0123) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0124) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0125) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0126) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0127) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0128) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0129) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0130) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0131) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0132) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0133) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0134) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0135) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0136) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0137) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0138) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0139) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0140) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0141) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0142) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0143) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0144) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0145) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0146) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0147) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0148) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0149) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0150) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0151) doesn't parse

BOOST_AUTO_TEST_SUITE_END()
