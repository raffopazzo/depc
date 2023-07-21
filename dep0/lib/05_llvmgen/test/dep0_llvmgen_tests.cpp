#define BOOST_TEST_MODULE dep0_llvmgen_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/llvmgen/gen.hpp"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

#include <filesystem>
#include <cstdlib>

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    llvm::LLVMContext llvm_ctx;

    boost::test_tools::predicate_result pass(std::filesystem::path const file)
    {
        auto parse_result = dep0::parser::parse(testfiles / file);
        if (parse_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), parse_result.error());
            return res;
        }
        auto check_result = dep0::typecheck::check(*parse_result);
        if (check_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), check_result.error());
            return res;
        }
        auto const gen_result = dep0::llvmgen::gen(llvm_ctx, "test.depc", *check_result);
        if (gen_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), gen_result.error());
            return res;
        }
        return true;
    }

    dep0::unique_ref<llvm::Module> gen(std::filesystem::path const file)
    {
        auto parse_result = dep0::parser::parse(testfiles / file);
        BOOST_TEST_REQUIRE(parse_result.has_value());
        auto check_result = dep0::typecheck::check(*parse_result);
        BOOST_TEST_REQUIRE(check_result.has_value());
        auto gen_result = dep0::llvmgen::gen(llvm_ctx, "test.depc", *check_result);
        BOOST_TEST_REQUIRE(gen_result.has_value());
        return std::move(*gen_result);
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000) { BOOST_TEST(pass("test_0000.depc")); }
BOOST_AUTO_TEST_CASE(test_0001) { BOOST_TEST(pass("test_0001.depc")); }
// BOOST_AUTO_TEST_CASE(test_0002) doesn't type check
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("test_0003.depc")); }
// BOOST_AUTO_TEST_CASE(test_0004) doesn't type check
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(pass("test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(pass("test_0007.depc")); }
// BOOST_AUTO_TEST_CASE(test_0008) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0009) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0010) doesn't type check
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(pass("test_0011.depc")); }
// BOOST_AUTO_TEST_CASE(test_0012) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0013) doesn't type check
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(pass("test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(pass("test_0015.depc")); }
// BOOST_AUTO_TEST_CASE(test_0016) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0017) doesn't type check
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(pass("test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(pass("test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(pass("test_0020.depc")); }
BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(pass("test_0021.depc")); }
BOOST_AUTO_TEST_CASE(test_0022) { BOOST_TEST(pass("test_0022.depc")); }
// BOOST_AUTO_TEST_CASE(test_0023) doesn't type check
BOOST_AUTO_TEST_CASE(test_0024) { BOOST_TEST(pass("test_0024.depc")); }
BOOST_AUTO_TEST_CASE(ignore_digit_separator_during_codegen)
{
    auto llvm_module = gen("test_0024.depc");
    auto* f235 = llvm_module->getFunction("f235");
    auto* f237 = llvm_module->getFunction("f237");
    auto* f238 = llvm_module->getFunction("f238");
    BOOST_TEST_REQUIRE(f235);
    BOOST_TEST_REQUIRE(f237);
    BOOST_TEST_REQUIRE(f238);
    auto* ret235 = cast<llvm::ReturnInst>(f235->getEntryBlock().getTerminator());
    auto* ret237 = cast<llvm::ReturnInst>(f237->getEntryBlock().getTerminator());
    auto* ret238 = cast<llvm::ReturnInst>(f238->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(ret235);
    BOOST_TEST_REQUIRE(ret237);
    BOOST_TEST_REQUIRE(ret238);
    auto* val235 = cast<llvm::ConstantInt>(ret235->getReturnValue());
    auto* val237 = cast<llvm::ConstantInt>(ret237->getReturnValue());
    auto* val238 = cast<llvm::ConstantInt>(ret238->getReturnValue());
    BOOST_TEST_REQUIRE(val235);
    BOOST_TEST_REQUIRE(val237);
    BOOST_TEST_REQUIRE(val238);
    BOOST_TEST(val235->getZExtValue() == 9999999999999999999ul);
    BOOST_TEST(val237->getZExtValue() == 9999999999999999999ul);
    BOOST_TEST(val238->getZExtValue() == 9999999999999999999ul);
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
