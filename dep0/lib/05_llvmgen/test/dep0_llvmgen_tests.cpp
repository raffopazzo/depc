#define BOOST_TEST_MODULE dep0_llvmgen_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/transform/beta_delta_reduction.hpp"

#include "dep0/llvmgen/gen.hpp"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

#include <boost/type_index.hpp>

#include <filesystem>
#include <cstdlib>
#include <ranges>

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    llvm::LLVMContext llvm_ctx;
    std::optional<dep0::unique_ref<llvm::Module>> pass_result;
    bool apply_beta_delta_normalization = false;

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
        if (apply_beta_delta_normalization)
        {
            dep0::transform::beta_delta_normalization_t beta_delta_normalization{};
            boost::ignore_unused(beta_delta_normalization(*check_result));
        }
        auto gen_result = dep0::llvmgen::gen(llvm_ctx, "test.depc", *check_result);
        if (gen_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), gen_result.error());
            return res;
        }
        pass_result.emplace(std::move(*gen_result));
        return true;
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000) { BOOST_TEST(pass("test_0000.depc")); }
BOOST_AUTO_TEST_CASE(test_0001) { BOOST_TEST(pass("test_0001.depc")); }
// BOOST_AUTO_TEST_CASE(test_0002) doesn't type check
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("test_0003.depc")); }
// BOOST_AUTO_TEST_CASE(test_0004) doesn't type check
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("test_0005.depc")); }
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
BOOST_AUTO_TEST_CASE(test_0024)
{
    BOOST_TEST_REQUIRE(pass("test_0024.depc"));
    auto* f0 = pass_result.value()->getFunction("f0");
    auto* f18 = pass_result.value()->getFunction("f18");
    auto* f42 = pass_result.value()->getFunction("f42");
    auto* f81 = pass_result.value()->getFunction("f81");
    auto* f141 = pass_result.value()->getFunction("f141");
    auto* f153 = pass_result.value()->getFunction("f153");
    auto* f169 = pass_result.value()->getFunction("f169");
    auto* f195 = pass_result.value()->getFunction("f195");
    auto* f235 = pass_result.value()->getFunction("f235");
    auto* f237 = pass_result.value()->getFunction("f237");
    auto* f238 = pass_result.value()->getFunction("f238");
    BOOST_TEST_REQUIRE(f0);
    BOOST_TEST_REQUIRE(f18);
    BOOST_TEST_REQUIRE(f42);
    BOOST_TEST_REQUIRE(f81);
    BOOST_TEST_REQUIRE(f141);
    BOOST_TEST_REQUIRE(f153);
    BOOST_TEST_REQUIRE(f169);
    BOOST_TEST_REQUIRE(f195);
    BOOST_TEST_REQUIRE(f235);
    BOOST_TEST_REQUIRE(f237);
    BOOST_TEST_REQUIRE(f238);
    BOOST_TEST(f0->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    BOOST_TEST(f18->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    BOOST_TEST(f42->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    BOOST_TEST(f81->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    BOOST_TEST(f141->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    BOOST_TEST(f153->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    BOOST_TEST(f169->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    BOOST_TEST(f195->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    BOOST_TEST(f235->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    BOOST_TEST(f237->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    BOOST_TEST(f238->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    auto* ret0 = cast<llvm::ReturnInst>(f0->getEntryBlock().getTerminator());
    auto* ret18 = cast<llvm::ReturnInst>(f18->getEntryBlock().getTerminator());
    auto* ret42 = cast<llvm::ReturnInst>(f42->getEntryBlock().getTerminator());
    auto* ret81 = cast<llvm::ReturnInst>(f81->getEntryBlock().getTerminator());
    auto* ret141 = cast<llvm::ReturnInst>(f141->getEntryBlock().getTerminator());
    auto* ret153 = cast<llvm::ReturnInst>(f153->getEntryBlock().getTerminator());
    auto* ret169 = cast<llvm::ReturnInst>(f169->getEntryBlock().getTerminator());
    auto* ret195 = cast<llvm::ReturnInst>(f195->getEntryBlock().getTerminator());
    auto* ret235 = cast<llvm::ReturnInst>(f235->getEntryBlock().getTerminator());
    auto* ret237 = cast<llvm::ReturnInst>(f237->getEntryBlock().getTerminator());
    auto* ret238 = cast<llvm::ReturnInst>(f238->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(ret0);
    BOOST_TEST_REQUIRE(ret18);
    BOOST_TEST_REQUIRE(ret42);
    BOOST_TEST_REQUIRE(ret81);
    BOOST_TEST_REQUIRE(ret141);
    BOOST_TEST_REQUIRE(ret153);
    BOOST_TEST_REQUIRE(ret169);
    BOOST_TEST_REQUIRE(ret195);
    BOOST_TEST_REQUIRE(ret235);
    BOOST_TEST_REQUIRE(ret237);
    BOOST_TEST_REQUIRE(ret238);
    auto* val0 = cast<llvm::ConstantInt>(ret0->getReturnValue());
    auto* val18 = cast<llvm::ConstantInt>(ret18->getReturnValue());
    auto* val42 = cast<llvm::ConstantInt>(ret42->getReturnValue());
    auto* val81 = cast<llvm::ConstantInt>(ret81->getReturnValue());
    auto* val141 = cast<llvm::ConstantInt>(ret141->getReturnValue());
    auto* val153 = cast<llvm::ConstantInt>(ret153->getReturnValue());
    auto* val169 = cast<llvm::ConstantInt>(ret169->getReturnValue());
    auto* val195 = cast<llvm::ConstantInt>(ret195->getReturnValue());
    auto* val235 = cast<llvm::ConstantInt>(ret235->getReturnValue());
    auto* val237 = cast<llvm::ConstantInt>(ret237->getReturnValue());
    auto* val238 = cast<llvm::ConstantInt>(ret238->getReturnValue());
    BOOST_TEST_REQUIRE(val0);
    BOOST_TEST_REQUIRE(val18);
    BOOST_TEST_REQUIRE(val42);
    BOOST_TEST_REQUIRE(val81);
    BOOST_TEST_REQUIRE(val141);
    BOOST_TEST_REQUIRE(val153);
    BOOST_TEST_REQUIRE(val169);
    BOOST_TEST_REQUIRE(val195);
    BOOST_TEST_REQUIRE(val235);
    BOOST_TEST_REQUIRE(val237);
    BOOST_TEST_REQUIRE(val238);
    BOOST_TEST(val0->getSExtValue() == 127);
    BOOST_TEST(val18->getSExtValue() == 32767);
    BOOST_TEST(val42->getSExtValue() == 2147483647);
    BOOST_TEST(val81->getSExtValue() == 9223372036854775807);
    BOOST_TEST(val141->getZExtValue() == 255ul);
    BOOST_TEST(val153->getZExtValue() == 65535ul);
    BOOST_TEST(val169->getZExtValue() == 4294967295ul);
    BOOST_TEST(val195->getZExtValue() == 18446744073709551615ul);
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
BOOST_AUTO_TEST_CASE(test_0152)
{
    BOOST_TEST_REQUIRE(pass("test_0152.depc"));
    auto* f_h = pass_result.value()->getFunction("h");
    auto* f_n = pass_result.value()->getFunction("n");
    auto* f_d = pass_result.value()->getFunction("d");
    BOOST_TEST_REQUIRE(f_h);
    BOOST_TEST_REQUIRE(f_n);
    BOOST_TEST_REQUIRE(f_d);
    BOOST_TEST(f_h->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    BOOST_TEST(f_n->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    BOOST_TEST(f_d->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    auto* r_h = cast<llvm::ReturnInst>(f_h->getEntryBlock().getTerminator());
    auto* r_n = cast<llvm::ReturnInst>(f_n->getEntryBlock().getTerminator());
    auto* r_d = cast<llvm::ReturnInst>(f_d->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r_h);
    BOOST_TEST_REQUIRE(r_n);
    BOOST_TEST_REQUIRE(r_d);
    auto* v_h = cast<llvm::ConstantInt>(r_h->getReturnValue());
    auto* v_n = cast<llvm::ConstantInt>(r_n->getReturnValue());
    auto* v_d = cast<llvm::ConstantInt>(r_d->getReturnValue());
    BOOST_TEST_REQUIRE(v_h);
    BOOST_TEST_REQUIRE(v_n);
    BOOST_TEST_REQUIRE(v_d);
    BOOST_TEST(v_h->getType()->getBitWidth() == 8);
    BOOST_TEST(v_n->getType()->getBitWidth() == 32);
    BOOST_TEST(v_d->getType()->getBitWidth() == 64);
    BOOST_TEST(v_h->getZExtValue() == 23ul);
    BOOST_TEST(v_n->getZExtValue() == 123456789ul);
    BOOST_TEST(v_d->getSExtValue() == -1234567890123456789l);
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
    BOOST_TEST_REQUIRE(pass("test_0163.depc"));
    auto* f = pass_result.value()->getFunction("zero");
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    auto* r = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r);
    auto* v = cast<llvm::ConstantInt>(r->getReturnValue());
    BOOST_TEST_REQUIRE(v);
    BOOST_TEST(v->getSExtValue() == 0);
    BOOST_TEST(v->getType()->getBitWidth() == 8);
}

BOOST_AUTO_TEST_CASE(test_0164)
{
    BOOST_TEST_REQUIRE(pass("test_0164.depc"));
    auto* f = pass_result.value()->getFunction("zero");
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    auto* r = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r);
    auto* v = cast<llvm::ConstantInt>(r->getReturnValue());
    BOOST_TEST_REQUIRE(v);
    BOOST_TEST(v->getZExtValue() == 0);
    BOOST_TEST(v->getType()->getBitWidth() == 8);
}
// BOOST_AUTO_TEST_CASE(test_0165) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0166) doesn't parse
BOOST_AUTO_TEST_CASE(test_0167)
{
    BOOST_TEST_REQUIRE(pass("test_0167.depc"));
    auto* f = pass_result.value()->getFunction("id");
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
    auto* x = f->getArg(0ul);
    BOOST_TEST(x->getName().str() == "x");
    BOOST_TEST(x->hasSExtAttr());
    auto* r = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r);
    BOOST_TEST(r->getReturnValue()->getName().str() == "x");
}
BOOST_AUTO_TEST_CASE(test_0168)
{
    BOOST_TEST_REQUIRE(pass("test_0168.depc"));
    auto* f1 = pass_result.value()->getFunction("id");
    auto* f2 = pass_result.value()->getFunction("main");
    BOOST_TEST_REQUIRE(f1);
    BOOST_TEST_REQUIRE(f2);
    auto *r = cast<llvm::ReturnInst>(f2->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r);
    auto* call = cast<llvm::CallInst>(r->getReturnValue());
    BOOST_TEST_REQUIRE(call);
    BOOST_TEST_REQUIRE(call->getCalledFunction() == f1);
    BOOST_TEST_REQUIRE(call->arg_size() == 1ul);
    BOOST_TEST(call->paramHasAttr(0, llvm::Attribute::SExt));
}
BOOST_AUTO_TEST_CASE(test_0169) { BOOST_TEST(pass("test_0169.depc")); }
// BOOST_AUTO_TEST_CASE(test_0170) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0171) doesn't type check
BOOST_AUTO_TEST_CASE(test_0172)
{
    BOOST_TEST_REQUIRE(pass("test_0172.depc"));
    auto* f1 = pass_result.value()->getFunction("first");
    auto* f2 = pass_result.value()->getFunction("main");
    BOOST_TEST_REQUIRE(f1);
    BOOST_TEST_REQUIRE(f2);
    auto const& b = f2->getEntryBlock();
    BOOST_TEST_REQUIRE(b.size() == 2ul);
    auto* call = cast<llvm::CallInst>(&*b.begin());
    BOOST_TEST_REQUIRE(call);
    BOOST_TEST_REQUIRE(call->getCalledFunction() == f1);
    BOOST_TEST_REQUIRE(call->arg_size() == 2ul);
    BOOST_TEST(call->paramHasAttr(0, llvm::Attribute::SExt));
    BOOST_TEST(call->paramHasAttr(1, llvm::Attribute::ZExt));
    auto* arg0 = cast<llvm::ConstantInt>(*call->arg_begin());
    auto* arg1 = cast<llvm::ConstantInt>(*std::next(call->arg_begin()));
    BOOST_TEST_REQUIRE(arg0);
    BOOST_TEST_REQUIRE(arg1);
    BOOST_TEST(arg0->getSExtValue() == 0);
    BOOST_TEST(arg1->getSExtValue() == 1);
}

BOOST_AUTO_TEST_CASE(test_0173)
{
    BOOST_TEST_REQUIRE(pass("test_0173.depc"));
    auto* f = pass_result.value()->getFunction("three");
    BOOST_TEST_REQUIRE(f);
    auto const& b = f->getEntryBlock();
    BOOST_TEST_REQUIRE(b.size() == 1ul);
    auto* r = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r);
    auto* v = cast<llvm::ConstantInt>(r->getReturnValue());
    BOOST_TEST_REQUIRE(v);
    BOOST_TEST(v->getSExtValue() == 3);
}

BOOST_AUTO_TEST_CASE(test_0174)
{
    BOOST_TEST_REQUIRE(pass("test_0174.depc"));
    auto* f = pass_result.value()->getFunction("three");
    BOOST_TEST_REQUIRE(f);
    auto const& b = f->getEntryBlock();
    BOOST_TEST_REQUIRE(b.size() == 1ul);
    auto* r = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r);
    auto* v = cast<llvm::ConstantInt>(r->getReturnValue());
    BOOST_TEST_REQUIRE(v);
    BOOST_TEST(v->getSExtValue() == 3);
}

BOOST_AUTO_TEST_CASE(test_0175)
{
    BOOST_TEST_REQUIRE(pass("test_0175.depc"));
    auto* f = pass_result.value()->getFunction("three");
    BOOST_TEST_REQUIRE(f);
    auto const& b = f->getEntryBlock();
    BOOST_TEST_REQUIRE(b.size() == 1ul);
    auto* r = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r);
    auto* v = cast<llvm::ConstantInt>(r->getReturnValue());
    BOOST_TEST_REQUIRE(v);
    BOOST_TEST(v->getSExtValue() == 3);
}

BOOST_AUTO_TEST_CASE(test_0176)
{
    BOOST_TEST_REQUIRE(pass("test_0176.depc"));
    auto* f = pass_result.value()->getFunction("minus_one");
    BOOST_TEST_REQUIRE(f);
    auto const& b = f->getEntryBlock();
    BOOST_TEST_REQUIRE(b.size() == 1ul);
    auto* r = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(r);
    auto* v = cast<llvm::ConstantInt>(r->getReturnValue());
    BOOST_TEST_REQUIRE(v);
    BOOST_TEST(v->getSExtValue() == -1);
}

// BOOST_AUTO_TEST_CASE(test_0177) doesn't type check

BOOST_AUTO_TEST_CASE(test_0178)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0178.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg_x = f->arg_begin();
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST(ret->getReturnValue()->getName().str() == "x");
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg_x = f->arg_begin();
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::ZExt));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST(ret->getReturnValue()->getName().str() == "x");
    }
    {
        auto const f = pass_result.value()->getFunction("h");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg_x = f->arg_begin();
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST(ret->getReturnValue()->getName().str() == "x");
    }
    {
        auto const f = pass_result.value()->getFunction("foo");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isMinusOne());
    }
    {
        auto const f = pass_result.value()->getFunction("bar");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isOne());
    }
    {
        auto const f = pass_result.value()->getFunction("baz");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isMinusOne());
    }
    {
        auto const f = pass_result.value()->getFunction("apply_f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isMinusOne());
    }
    {
        auto const f = pass_result.value()->getFunction("apply_g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isOne());
    }
    {
        auto const f = pass_result.value()->getFunction("apply_h");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isOne());
    }
    {
        auto const f = pass_result.value()->getFunction("apply_0");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg = f->arg_begin();
        BOOST_TEST(arg->getName().str() == "apply");
        BOOST_TEST_REQUIRE(arg->getType()->isPointerTy());
        auto const arg_type = cast<llvm::FunctionType>(arg->getType()->getPointerElementType());
        BOOST_TEST_REQUIRE(arg_type);
        BOOST_TEST(arg_type->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST_REQUIRE(arg_type->getNumParams() == 1ul);
        BOOST_TEST(arg_type->getParamType(0ul)->isIntegerTy(32ul));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 2ul);
        auto const call = cast<llvm::CallInst>(&*f->getEntryBlock().begin());
        BOOST_TEST_REQUIRE(call);
        BOOST_TEST(call->isIndirectCall());
        BOOST_TEST(call->getCalledOperand() == arg);
        BOOST_TEST_REQUIRE(call->arg_size() == 1ul);
        BOOST_TEST(call->paramHasAttr(0, llvm::Attribute::SExt));
        auto const val = cast<llvm::ConstantInt>(call->arg_begin()->get());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isZero());
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST(ret->getReturnValue() == call);
    }
    {
        auto const f = pass_result.value()->getFunction("discard_id_v1");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isZero());
    }
    {
        auto const f = pass_result.value()->getFunction("discard_id_v2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isZero());
    }
    {
        auto const f = pass_result.value()->getFunction("multi_f_i32");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isZero());
    }
    {
        auto const f = pass_result.value()->getFunction("apply_id_v1");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg_x = f->arg_begin();
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST(ret->getReturnValue()->getName().str() == "x");
    }
    {
        auto const f = pass_result.value()->getFunction("apply_id_v2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg_x = f->arg_begin();
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST(ret->getReturnValue()->getName().str() == "x");
    }
}

// BOOST_AUTO_TEST_CASE(test_0179) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0180) doesn't type check

BOOST_AUTO_TEST_CASE(test_0181)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0181.depc"));
    auto const f = pass_result.value()->getFunction("g");
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
    BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
    BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
    auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(ret);
    auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
    BOOST_TEST_REQUIRE(val);
    BOOST_TEST(val->isZero());
}

// BOOST_AUTO_TEST_CASE(test_0182) doesn't type check

BOOST_AUTO_TEST_CASE(test_0183)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0183.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 2ul);
        auto const arg_x = f->getArg(0);
        BOOST_TEST(arg_x->getName().str() == "x");
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        auto const arg_y = f->getArg(1);
        BOOST_TEST(arg_y->getName().str() == "y");
        BOOST_TEST(arg_y->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_y->hasAttribute(llvm::Attribute::SExt));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST(ret->getReturnValue()->getName().str() == "x");
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 2ul);
        auto const arg_x = f->getArg(0);
        BOOST_TEST(arg_x->getName().str() == "x");
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        auto const arg_y = f->getArg(1);
        BOOST_TEST(arg_y->getName().str() == "y");
        BOOST_TEST(arg_y->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_y->hasAttribute(llvm::Attribute::SExt));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST(ret->getReturnValue()->getName().str() == "y");
    }
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isZero());
    }
}

// BOOST_AUTO_TEST_CASE(test_0184) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0185) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0186) doesn't type check

BOOST_AUTO_TEST_CASE(test_0187)
{
#if 1
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0187.depc"));
    auto const f = pass_result.value()->getFunction("main");
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
    BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
    BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
    auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(ret);
    auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
    BOOST_TEST_REQUIRE(val);
    BOOST_TEST(val->isZero());
#else
    BOOST_TEST_REQUIRE(pass("test_0187.depc"));
    {
        auto const f = pass_result.value()->getFunction("f.u32_t");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isZero());
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const call = cast<llvm::CallInst>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(call);
        BOOST_TEST(call->getCalledFunction() == pass_result.value()->getFunction("f.u32_t"));
    }
#endif
}

BOOST_AUTO_TEST_CASE(test_0188)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0188.depc"));
    auto const f = pass_result.value()->getFunction("main");
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
    BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
    BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
    auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
    BOOST_TEST_REQUIRE(ret);
    auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
    BOOST_TEST_REQUIRE(val);
    BOOST_TEST(val->isZero());
}

BOOST_AUTO_TEST_CASE(test_0189)
{
    BOOST_TEST_REQUIRE(pass("test_0189.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg = f->getArg(0);
        BOOST_TEST(arg->getName().empty());
        BOOST_TEST(arg->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg->hasAttribute(llvm::Attribute::SExt));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isZero());
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg = f->getArg(0);
        BOOST_TEST(arg->getName().str() == "h");
        BOOST_TEST_REQUIRE(arg->getType()->isPointerTy());
        auto const arg_type = cast<llvm::FunctionType>(arg->getType()->getPointerElementType());
        BOOST_TEST_REQUIRE(arg_type);
        BOOST_TEST(arg_type->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST_REQUIRE(arg_type->getNumParams() == 1ul);
        BOOST_TEST(arg_type->getParamType(0ul)->isIntegerTy(32ul));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const call = cast<llvm::CallInst>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(call);
        BOOST_TEST(call->isIndirectCall());
        BOOST_TEST(call->getCalledOperand() == arg);
        BOOST_TEST_REQUIRE(call->arg_size() == 1ul);
        BOOST_TEST(call->paramHasAttr(0, llvm::Attribute::SExt));
        auto const call_arg = cast<llvm::ConstantInt>(*call->arg_begin());
        BOOST_TEST_REQUIRE(call_arg);
        BOOST_TEST(call_arg->getSExtValue() == 1);
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const call = cast<llvm::CallInst>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(call);
        BOOST_TEST(call->getCalledFunction() == pass_result.value()->getFunction("g"));
        BOOST_TEST_REQUIRE(call->arg_size() == 1ul);
        BOOST_TEST(*call->arg_begin() == pass_result.value()->getFunction("f"));
    }
}

BOOST_AUTO_TEST_CASE(test_0190)
{
    BOOST_TEST_REQUIRE(pass("test_0190.depc"));
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const val = cast<llvm::ConstantInt>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(val);
        BOOST_TEST(val->isZero());
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg = f->getArg(0);
        BOOST_TEST(arg->getName().str() == "f");
        BOOST_TEST_REQUIRE(arg->getType()->isPointerTy());
        auto const arg_type = cast<llvm::FunctionType>(arg->getType()->getPointerElementType());
        BOOST_TEST_REQUIRE(arg_type);
        BOOST_TEST(arg_type->getNumParams() == 0ul);
        BOOST_TEST(arg_type->getReturnType()->isIntegerTy(32ul));
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const call = cast<llvm::CallInst>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(call);
        BOOST_TEST(call->isIndirectCall());
        BOOST_TEST(call->getCalledOperand() == arg);
        BOOST_TEST(call->arg_size() == 0ul);
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 0ul);
        auto const ret = cast<llvm::ReturnInst>(f->getEntryBlock().getTerminator());
        BOOST_TEST_REQUIRE(ret);
        auto const call = cast<llvm::CallInst>(ret->getReturnValue());
        BOOST_TEST_REQUIRE(call);
        BOOST_TEST(call->getCalledFunction() == pass_result.value()->getFunction("g"));
        BOOST_TEST_REQUIRE(call->arg_size() == 1ul);
        BOOST_TEST(*call->arg_begin() == pass_result.value()->getFunction("zero"));
    }
}

BOOST_AUTO_TEST_SUITE_END()
