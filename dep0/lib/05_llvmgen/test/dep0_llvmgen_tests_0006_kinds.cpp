#define BOOST_TEST_MODULE dep0_llvmgen_tests_0006_kinds
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0006_kinds, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_000.depc"));
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

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_001.depc"));
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

BOOST_AUTO_TEST_CASE(pass_002)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_002.depc"));
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
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(1ul));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_bool(f->getArg(0ul), "x"));
        BOOST_TEST_REQUIRE(f->size() == 3ul);
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_branch_of(f->getEntryBlock().getTerminator(), [&] (llvm::BranchInst const& br)
        {
            BOOST_TEST_REQUIRE(br.isConditional());
            BOOST_TEST(br.getCondition() == f->getArg(0ul));
            BOOST_TEST(is_return_of(br.getSuccessor(0)->getTerminator(), unsigned_constant(0)));
            BOOST_TEST(is_return_of(br.getSuccessor(1)->getTerminator(), unsigned_constant(1)));
            return boost::test_tools::predicate_result(true);
        }));
    }
    {
        auto const f = pass_result.value()->getFunction("plus_1");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(exactly<llvm::Value>(f->getArg(0ul)), signed_constant(1))));
    }
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(2)));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(2)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(f->getAttributes().isEmpty());
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("g2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(f->getAttributes().isEmpty());
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(1)));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("negate");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(1ul));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_bool(f->getArg(0ul), "x"));
        BOOST_TEST_REQUIRE(f->size() == 3ul);
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_branch_of(f->getEntryBlock().getTerminator(), [&] (llvm::BranchInst const& br)
        {
            BOOST_TEST_REQUIRE(br.isConditional());
            BOOST_TEST(br.getCondition() == f->getArg(0ul));
            BOOST_TEST(is_return_of(br.getSuccessor(0)->getTerminator(), unsigned_constant(0)));
            BOOST_TEST(is_return_of(br.getSuccessor(1)->getTerminator(), unsigned_constant(1)));
            return boost::test_tools::predicate_result(true);
        }));
    }
    {
        auto const f = pass_result.value()->getFunction("to_bool");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(not f->getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), std::nullopt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("to_int");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_bool(f->getArg(0ul), std::nullopt));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("zero_from_int");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("zero_from_bool");
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
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("zero_int");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("zero_int_2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("zero_bool");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(not f->getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("zero_bool_2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(not f->getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), unsigned_constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_006.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isVoidTy());
        BOOST_TEST(not f->getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isVoidTy());
        BOOST_TEST(not f->getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_007.depc"));
    {
        auto const f = pass_result.value()->getFunction("succ");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->getAttributes().hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 2ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(exactly<llvm::Value>(f->getArg(0ul)), signed_constant(1))));
    }
    {
        auto const f = pass_result.value()->getFunction("apply_0");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->getAttributes().hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const arg = f->getArg(0ul);
        BOOST_TEST(arg->getName().str() == "f");
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
        auto const f = pass_result.value()->getFunction("one");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f->getAttributes().hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), signed_constant(1)));
    }
}

BOOST_AUTO_TEST_SUITE_END()
