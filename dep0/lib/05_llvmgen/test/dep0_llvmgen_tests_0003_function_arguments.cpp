#define BOOST_TEST_MODULE dep0_llvmgen_tests_0003_function_arguments
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0003_function_arguments, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_000.depc"));
    BOOST_TEST(has_function("id", [] (llvm::Function const& f)
    {
        BOOST_TEST_REQUIRE(f.arg_size() == 1ul);
        BOOST_TEST(is_i32(f.getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f.getEntryBlock().getTerminator(), "x"));
        return boost::test_tools::predicate_result(true);
    }));
}
BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_001.depc"));
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
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0003_function_arguments/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_003.depc"));
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

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)

BOOST_AUTO_TEST_SUITE_END()
