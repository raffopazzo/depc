#define BOOST_TEST_MODULE dep0_llvmgen_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/transform/beta_delta_normalization.hpp"

#include "dep0/llvmgen/gen.hpp"

#include "dep0/testing/predicate.hpp"
#include "dep0/testing/failure.hpp"

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

    template <dep0::testing::Predicate<llvm::Function> F>
    boost::test_tools::predicate_result has_function(std::string_view const name, F&& f)
    {
        auto const function = pass_result.value()->getFunction(name);
        if (not function)
            return dep0::testing::failure("function not found: ", name);
        return std::forward<F>(f)(*function);
    }

    template <dep0::testing::Predicate<llvm::FunctionType> F>
    static boost::test_tools::predicate_result is_pointer_to_function(llvm::Type const* const type, F&& f)
    {
        if (not type)
            return dep0::testing::failure("type is null");
        if (not type->isPointerTy())
            return dep0::testing::failure("type is not a pointer");
        auto const fn_type = cast<llvm::FunctionType>(type->getPointerElementType());
        if (not fn_type)
            return dep0::testing::failure("type is not a pointer to function");
        return std::forward<F>(f)(*fn_type);
    }

    static boost::test_tools::predicate_result is_return_of(
        llvm::Instruction const* const instr,
        std::string_view const name)
    {
        if (not instr)
            return dep0::testing::failure("instr is null");
        auto const ret = cast<llvm::ReturnInst>(instr);
        if (not ret)
            return dep0::testing::failure("not a return instruction");
        if (not ret->getReturnValue())
            return dep0::testing::failure("return instruction has no value");
        if (auto const& str = ret->getReturnValue()->getName().str(); str != name)
            return dep0::testing::failure("return value ", str, " != ", name);
        return true;
    }

    template <dep0::testing::Predicate<llvm::Value> F>
    static boost::test_tools::predicate_result is_return_of(llvm::Instruction const* const instr, F&& f)
    {
        if (not instr)
            return dep0::testing::failure("instr is null");
        auto const ret = cast<llvm::ReturnInst>(instr);
        if (not ret)
            return dep0::testing::failure("not a return instruction");
        if (not ret->getReturnValue())
            return dep0::testing::failure("return instruction has no value");
        return std::forward<F>(f)(*ret->getReturnValue());
    }

    template <dep0::testing::Predicate<llvm::CallInst> F>
    static boost::test_tools::predicate_result is_call_inst(llvm::Instruction const& instr, F&& f)
    {
        auto const call = cast<llvm::CallInst>(&instr);
        if (not call)
            return dep0::testing::failure("not a call instruction");
        return std::forward<F>(f)(*call);
    }

    template <dep0::testing::Predicate<llvm::CallInst> F>
    static auto call_inst(F&& f)
    {
        return [f=std::forward<F>(f)] (llvm::Value const& v) -> boost::test_tools::predicate_result
        {
            auto const call = cast<llvm::CallInst>(&v);
            if (not call)
                return dep0::testing::failure("value is not a function call");
            return f(*call);
        };
    }

    template <dep0::testing::Predicate<llvm::ConstantInt> F>
    static auto constant_int(F&& f)
    {
        return [f=std::forward<F>(f)] (llvm::Value const& v) -> boost::test_tools::predicate_result
        {
            auto const c = cast<llvm::ConstantInt>(&v);
            if (not c)
                return dep0::testing::failure("value is not a constant");
            return f(*c);
        };
    }

    static boost::test_tools::predicate_result is_signed_constant(llvm::Value const& v, long long const value)
    {
        auto const c = cast<llvm::ConstantInt>(&v);
        if (not c)
            return dep0::testing::failure("value is not a constant");
        if (c->getSExtValue() != value)
            return dep0::testing::failure(c->getSExtValue(), " != ", value);
        return true;
    }

    static auto signed_constant(long long const value)
    {
        return [value] (llvm::Value const& v) -> boost::test_tools::predicate_result
        {
            auto const c = cast<llvm::ConstantInt>(&v);
            if (not c)
                return dep0::testing::failure("value is not a constant");
            if (c->getSExtValue() != value)
                return dep0::testing::failure(c->getSExtValue(), " != ", value);
            return true;
        };
    }

    static auto unsigned_constant(unsigned long long const value)
    {
        return [value] (llvm::Value const& v) -> boost::test_tools::predicate_result
        {
            auto const c = cast<llvm::ConstantInt>(&v);
            if (not c)
                return dep0::testing::failure("value is not a constant");
            if (c->getZExtValue() != value)
                return dep0::testing::failure(c->getZExtValue(), " != ", value);
            return true;
        };
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
BOOST_AUTO_TEST_CASE(test_0152)
{
    BOOST_TEST_REQUIRE(pass("test_0152.depc"));
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
    BOOST_TEST_REQUIRE(pass("test_0163.depc"));
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
    BOOST_TEST_REQUIRE(pass("test_0164.depc"));
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
BOOST_AUTO_TEST_CASE(test_0167)
{
    BOOST_TEST_REQUIRE(pass("test_0167.depc"));
    BOOST_TEST(has_function("id", [] (llvm::Function const& f)
    {
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const x = f.getArg(0ul);
        BOOST_TEST(x->getName().str() == "x");
        BOOST_TEST(x->hasSExtAttr());
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
}
BOOST_AUTO_TEST_CASE(test_0168)
{
    BOOST_TEST_REQUIRE(pass("test_0168.depc"));
    BOOST_TEST_REQUIRE(pass_result.value()->getFunction("id"));
    BOOST_TEST(has_function("main", [id=pass_result.value()->getFunction("id")] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), call_inst([id] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == id);
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.paramHasAttr(0, llvm::Attribute::SExt));
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
}
BOOST_AUTO_TEST_CASE(test_0169) { BOOST_TEST(pass("test_0169.depc")); }
// BOOST_AUTO_TEST_CASE(test_0170) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0171) doesn't type check
BOOST_AUTO_TEST_CASE(test_0172)
{
    BOOST_TEST_REQUIRE(pass("test_0172.depc"));
    BOOST_TEST_REQUIRE(pass_result.value()->getFunction("first"));
    BOOST_TEST(has_function("main", [f1=pass_result.value()->getFunction("first")] (llvm::Function const& f2)
    {
        auto const& b = f2.getEntryBlock();
        BOOST_TEST_REQUIRE(b.size() == 2ul);
        BOOST_TEST(is_call_inst(*b.begin(), [f1] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == f1);
            BOOST_TEST_REQUIRE(call.arg_size() == 2ul);
            BOOST_TEST(call.paramHasAttr(0, llvm::Attribute::SExt));
            BOOST_TEST(call.paramHasAttr(1, llvm::Attribute::ZExt));
            BOOST_TEST(is_signed_constant(*call.getArgOperand(0), 0));
            BOOST_TEST(is_signed_constant(*call.getArgOperand(1), 1));
            return boost::test_tools::predicate_result(true);
        }));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0173)
{
    BOOST_TEST_REQUIRE(pass("test_0173.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0174)
{
    BOOST_TEST_REQUIRE(pass("test_0174.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0175)
{
    BOOST_TEST_REQUIRE(pass("test_0175.depc"));
    BOOST_TEST(has_function("three", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(3)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0176)
{
    BOOST_TEST_REQUIRE(pass("test_0176.depc"));
    BOOST_TEST(has_function("minus_one", [] (llvm::Function const& f)
    {
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(-1)));
        return boost::test_tools::predicate_result(true);
    }));
}

// BOOST_AUTO_TEST_CASE(test_0177) doesn't type check

BOOST_AUTO_TEST_CASE(test_0178)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0178.depc"));
    BOOST_TEST(has_function("f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg_x = f.getArg(0ul);
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg_x = f.getArg(0ul);
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("h", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg_x = f.getArg(0ul);
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("foo", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(-1)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("bar", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(1)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("baz", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(-1)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("apply_f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(-1)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("apply_g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(1)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("apply_h", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(1)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("apply_0", [] (llvm::Function const& f)
    {
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg = f.getArg(0ul);
        BOOST_TEST(arg->getName().str() == "apply");
        BOOST_TEST(is_pointer_to_function(arg->getType(), [] (llvm::FunctionType const& t)
        {
            BOOST_TEST(t.getReturnType()->isIntegerTy(32ul));
            BOOST_TEST_REQUIRE(t.getNumParams() == 1ul);
            BOOST_TEST(t.getParamType(0ul)->isIntegerTy(32ul));
            return boost::test_tools::predicate_result(true);
        }));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), call_inst([arg] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.isIndirectCall());
            BOOST_TEST(call.getCalledOperand() == arg);
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.paramHasAttr(0, llvm::Attribute::SExt));
            BOOST_TEST(is_signed_constant(*call.getArgOperand(0), 0));
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("discard_id_v1", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("discard_id_v2", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("multi_f_i32", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("apply_id_v1", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg_x = f.getArg(0ul);
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("apply_id_v2", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg_x = f.getArg(0ul);
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
}

// BOOST_AUTO_TEST_CASE(test_0179) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0180) doesn't type check

BOOST_AUTO_TEST_CASE(test_0181)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0181.depc"));
    BOOST_TEST(has_function("g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
}

// BOOST_AUTO_TEST_CASE(test_0182) doesn't type check

BOOST_AUTO_TEST_CASE(test_0183)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0183.depc"));
    BOOST_TEST(has_function("f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 2ul);
        auto const arg_x = f.getArg(0);
        BOOST_TEST(arg_x->getName().str() == "x");
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        auto const arg_y = f.getArg(1);
        BOOST_TEST(arg_y->getName().str() == "y");
        BOOST_TEST(arg_y->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_y->hasAttribute(llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 2ul);
        auto const arg_x = f.getArg(0);
        BOOST_TEST(arg_x->getName().str() == "x");
        BOOST_TEST(arg_x->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_x->hasAttribute(llvm::Attribute::SExt));
        auto const arg_y = f.getArg(1);
        BOOST_TEST(arg_y->getName().str() == "y");
        BOOST_TEST(arg_y->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg_y->hasAttribute(llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "y"));
        return boost::test_tools::predicate_result(true);
    }));
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

// BOOST_AUTO_TEST_CASE(test_0184) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0185) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0186) doesn't type check

BOOST_AUTO_TEST_CASE(test_0187)
{
#if 1
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0187.depc"));
    BOOST_TEST(has_function("main", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
#else
    BOOST_TEST_REQUIRE(pass("test_0187.depc"));
    BOOST_TEST(has_function("f.u32_t", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("main", [this] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), call_inst([this] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == pass_result.value()->getFunction("f.u32_t"));
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
#endif
}

BOOST_AUTO_TEST_CASE(test_0188)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("test_0188.depc"));
    BOOST_TEST(has_function("main", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
       return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0189)
{
    BOOST_TEST_REQUIRE(pass("test_0189.depc"));
    BOOST_TEST(has_function("f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg = f.getArg(0);
        BOOST_TEST(arg->getName().empty());
        BOOST_TEST(arg->getType()->isIntegerTy(32ul));
        BOOST_TEST(arg->hasAttribute(llvm::Attribute::SExt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg = f.getArg(0);
        BOOST_TEST(arg->getName().str() == "h");
        BOOST_TEST(is_pointer_to_function(arg->getType(), [] (llvm::FunctionType const& t)
        {
            BOOST_TEST(t.getReturnType()->isIntegerTy(32ul));
            BOOST_TEST_REQUIRE(t.getNumParams() == 1ul);
            BOOST_TEST(t.getParamType(0ul)->isIntegerTy(32ul));
            return boost::test_tools::predicate_result(true);
        }));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), call_inst([arg] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.isIndirectCall());
            BOOST_TEST(call.getCalledOperand() == arg);
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.paramHasAttr(0, llvm::Attribute::SExt));
            BOOST_TEST(is_signed_constant(*call.getArgOperand(0), 1));
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("main", [this] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), call_inst([this] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == pass_result.value()->getFunction("g"));
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.getArgOperand(0) == pass_result.value()->getFunction("f"));
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(test_0190)
{
    BOOST_TEST_REQUIRE(pass("test_0190.depc"));
    BOOST_TEST(has_function("zero", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        auto const arg = f.getArg(0);
        BOOST_TEST(arg->getName().str() == "f");
        BOOST_TEST(is_pointer_to_function(arg->getType(), [] (llvm::FunctionType const& t)
        {
            BOOST_TEST(t.getNumParams() == 0ul);
            BOOST_TEST(t.getReturnType()->isIntegerTy(32ul));
            return boost::test_tools::predicate_result(true);
        }));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), call_inst([arg] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.isIndirectCall());
            BOOST_TEST(call.getCalledOperand() == arg);
            BOOST_TEST(call.arg_size() == 0ul);
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("main", [this] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), call_inst([this] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == pass_result.value()->getFunction("g"));
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.getArgOperand(0) == pass_result.value()->getFunction("zero"));
            return boost::test_tools::predicate_result(true);
        })));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_SUITE_END()
