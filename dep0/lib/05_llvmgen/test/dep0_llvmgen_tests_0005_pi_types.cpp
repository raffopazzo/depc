#define BOOST_TEST_MODULE dep0_llvmgen_tests_0005_pi_types
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0005_pi_types, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_000.depc"));
    BOOST_TEST(has_function("f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_i32(f.getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_u32(f.getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("h", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_i32(f.getArg(0ul), "x"));
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
        BOOST_TEST(is_i32(f.getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("apply_id_v2", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_i32(f.getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_001.depc"));
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

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_002.depc"));
    BOOST_TEST(has_function("f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 2ul);
        BOOST_TEST(is_i32(f.getArg(0ul), "x"));
        BOOST_TEST(is_i32(f.getArg(1ul), "y"));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 2ul);
        BOOST_TEST(is_i32(f.getArg(0ul), "x"));
        BOOST_TEST(is_i32(f.getArg(1ul), "y"));
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

BOOST_AUTO_TEST_CASE(pass_003)
{
#if 1
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_003.depc"));
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
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_003.depc"));
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

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_004.depc"));
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

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_005.depc"));
    BOOST_TEST(has_function("f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_i32(f.getArg(0ul), std::nullopt));
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

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_006.depc"));
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

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)
// BOOST_AUTO_TEST_CASE(typecheck_error_003)
// BOOST_AUTO_TEST_CASE(typecheck_error_004)
// BOOST_AUTO_TEST_CASE(typecheck_error_005)

BOOST_AUTO_TEST_SUITE_END()
