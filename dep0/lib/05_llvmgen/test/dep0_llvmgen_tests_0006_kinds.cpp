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

BOOST_AUTO_TEST_CASE(pass_003)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_003.depc"));
    BOOST_TEST(has_function("negate", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(1ul));
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_bool(f.getArg(0ul), "x"));
        BOOST_TEST_REQUIRE(f.size() == 3ul);
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(is_branch_of(f.getEntryBlock().getTerminator(), [&] (llvm::BranchInst const& br)
        {
            BOOST_TEST_REQUIRE(br.isConditional());
            BOOST_TEST(br.getCondition() == f.getArg(0ul));
            BOOST_TEST(is_return_of(br.getSuccessor(0)->getTerminator(), unsigned_constant(0)));
            BOOST_TEST(is_return_of(br.getSuccessor(1)->getTerminator(), unsigned_constant(1)));
            return boost::test_tools::predicate_result(true);
        }));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("plus_1", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_i32(f.getArg(0ul), "x"));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 2ul);
        auto const& bb = f.getEntryBlock();
        auto it = bb.begin();
        BOOST_TEST(is_add_of(&*it++, add_of([&] (llvm::BinaryOperator const& add)
        {
            BOOST_TEST_REQUIRE(add.getNumOperands() == 2ul);
            BOOST_TEST(add.getOperand(0ul) == f.getArg(0ul));
            BOOST_TEST(is_signed_constant(*add.getOperand(1ul), 1));
            return boost::test_tools::predicate_result(true);
        })));
        BOOST_TEST(is_return_of(&*it++, *bb.begin()));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(2)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("f2", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(2)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("g", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(f.getAttributes().isEmpty());
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(1)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("g2", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(f.getAttributes().isEmpty());
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(1)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_004.depc"));
    BOOST_TEST(has_function("negate", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(1ul));
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_bool(f.getArg(0ul), "x"));
        BOOST_TEST_REQUIRE(f.size() == 3ul);
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(is_branch_of(f.getEntryBlock().getTerminator(), [&] (llvm::BranchInst const& br)
        {
            BOOST_TEST_REQUIRE(br.isConditional());
            BOOST_TEST(br.getCondition() == f.getArg(0ul));
            BOOST_TEST(is_return_of(br.getSuccessor(0)->getTerminator(), unsigned_constant(0)));
            BOOST_TEST(is_return_of(br.getSuccessor(1)->getTerminator(), unsigned_constant(1)));
            return boost::test_tools::predicate_result(true);
        }));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("to_bool", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(not f.getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_i32(f.getArg(0ul), std::nullopt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("to_int", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_bool(f.getArg(0ul), std::nullopt));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("zero_from_int", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("zero_from_bool", [] (llvm::Function const& f)
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
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_005.depc"));
    BOOST_TEST(has_function("zero_int", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("zero_int_2", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(32ul));
        BOOST_TEST(f.hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), signed_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("zero_bool", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(not f.getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
    BOOST_TEST(has_function("zero_bool_2", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isIntegerTy(1ul));
        BOOST_TEST(not f.getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), unsigned_constant(0)));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_006.depc"));
    BOOST_TEST(has_function("f", [] (llvm::Function const& f)
    {
        BOOST_TEST(f.getReturnType()->isVoidTy());
        BOOST_TEST(not f.getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex));
        BOOST_TEST_REQUIRE(f.getEntryBlock().size() == 1ul);
        BOOST_TEST(f.arg_size() == 0ul);
        BOOST_TEST(is_return_of_void(f.getEntryBlock().getTerminator()));
        return boost::test_tools::predicate_result(true);
    }));
}

BOOST_AUTO_TEST_SUITE_END()
