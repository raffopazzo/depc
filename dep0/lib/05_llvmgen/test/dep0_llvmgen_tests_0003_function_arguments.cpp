#define BOOST_TEST_MODULE dep0_llvmgen_tests_0003_function_arguments
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0003_function_arguments, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("id");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(is_i32(f->getArg(0ul), "x"));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), "x"));
    }
}
BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_001.depc"));
    auto const id = pass_result.value()->getFunction("id");
    BOOST_TEST_REQUIRE(id);
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), call_inst([id] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == id);
            BOOST_TEST_REQUIRE(call.arg_size() == 1ul);
            BOOST_TEST(call.paramHasAttr(0, llvm::Attribute::SExt));
            return boost::test_tools::predicate_result(true);
        })));
    }
}
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0003_function_arguments/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_003.depc"));
    auto const first = pass_result.value()->getFunction("first");
    BOOST_TEST_REQUIRE(first);
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(f);
        auto const& b = f->getEntryBlock();
        BOOST_TEST_REQUIRE(b.size() == 2ul);
        BOOST_TEST(is_call_inst(*b.begin(), [first] (llvm::CallInst const& call)
        {
            BOOST_TEST(call.getCalledFunction() == first);
            BOOST_TEST_REQUIRE(call.arg_size() == 2ul);
            BOOST_TEST(call.paramHasAttr(0, llvm::Attribute::SExt));
            BOOST_TEST(call.paramHasAttr(1, llvm::Attribute::ZExt));
            BOOST_TEST(is_signed_constant(*call.getArgOperand(0), 0));
            BOOST_TEST(is_signed_constant(*call.getArgOperand(1), 1));
            return boost::test_tools::predicate_result(true);
        }));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("unit");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(has_void_return_type(*f));
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(has_void_return_type(*f));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(f->getArg(0ul)->getType()->isIntegerTy(8ul));
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
    {
        auto const f = pass_result.value()->getFunction("g1");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(has_void_return_type(*f));
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(has_void_return_type(*f));
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        BOOST_TEST(f->getArg(0ul)->getType()->isIntegerTy(8ul));
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
    {
        auto const f = pass_result.value()->getFunction("g2");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(has_void_return_type(*f));
        BOOST_TEST(f->arg_size() == 0ul);
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)

BOOST_AUTO_TEST_SUITE_END()
