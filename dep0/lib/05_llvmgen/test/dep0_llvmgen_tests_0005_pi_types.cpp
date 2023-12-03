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
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), "x"));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_u32(f->getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), "x"));
    }
    {
        auto const f = pass_result.value()->getFunction("h");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), "x"));
    }
    {
        auto const f = pass_result.value()->getFunction("foo");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("bar");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("baz");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("apply_f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("apply_g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("apply_h");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("apply_0");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg = f->getArg(0ul);
        BOOST_TEST(arg->getName().str() == "apply");
        BOOST_TEST(is_pointer_to_function(arg->getType(), std::tuple{is_i32_type}, is_i32_type));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), call_inst([arg] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.isIndirectCall());
            BOOST_TEST(call.getCalledOperand() == arg);
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.paramHasAttr(0, llvm::Attribute::SExt));
            BOOST_TEST(is_signed_constant(*call.getArgOperand(0), 0));
            return boost::test_tools::predicate_result(true);
        })));
    }
    {
        auto const f = pass_result.value()->getFunction("discard_id_v1");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("discard_id_v2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("multi_f_i32");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("apply_id_v1");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), "x"));
    }
    {
        auto const f = pass_result.value()->getFunction("apply_id_v2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), "x"));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 2ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST(is_i32(f->getArg(1ul), "y"));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), "x"));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 2ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST(is_i32(f->getArg(1ul), "y"));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), "y"));
    }
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
#if 1
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
#else
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("f->u32_t");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), call_inst([this] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == pass_result.value()->getFunction("f->u32_t"));
            return boost::test_tools::predicate_result(true);
        })));
    }
#endif
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), std::nullopt));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg = f->getArg(0);
        BOOST_TEST(arg->getName().str() == "h");
        BOOST_TEST(is_pointer_to_function(arg->getType(), std::tuple{is_i32_type}, is_i32_type));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), call_inst([arg] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.isIndirectCall());
            BOOST_TEST(call.getCalledOperand() == arg);
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.paramHasAttr(0, llvm::Attribute::SExt));
            BOOST_TEST(is_signed_constant(*call.getArgOperand(0), 1));
            return boost::test_tools::predicate_result(true);
        })));
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), call_inst([this] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == pass_result.value()->getFunction("g"));
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.getArgOperand(0) == pass_result.value()->getFunction("f"));
            return boost::test_tools::predicate_result(true);
        })));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_006.depc"));
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg = f->getArg(0);
        BOOST_TEST(arg->getName().str() == "f");
        BOOST_TEST(is_pointer_to_function(arg->getType(), std::tuple{}, is_i32_type));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), call_inst([arg] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.isIndirectCall());
            BOOST_TEST(call.getCalledOperand() == arg);
            BOOST_TEST(call.arg_size() == 0ul);
            return boost::test_tools::predicate_result(true);
        })));
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), call_inst([this] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == pass_result.value()->getFunction("g"));
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.getArgOperand(0) == pass_result.value()->getFunction("zero"));
            return boost::test_tools::predicate_result(true);
        })));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)
// BOOST_AUTO_TEST_CASE(typecheck_error_003)
// BOOST_AUTO_TEST_CASE(typecheck_error_004)
// BOOST_AUTO_TEST_CASE(typecheck_error_005)

BOOST_AUTO_TEST_SUITE_END()
