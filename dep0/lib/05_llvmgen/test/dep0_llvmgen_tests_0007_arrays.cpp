#define BOOST_TEST_MODULE dep0_llvmgen_tests_0007_arrays
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0007_arrays, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("values");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->getReturnType()->isVoidTy());
        BOOST_TEST_REQUIRE(f->arg_size() == 1ul);
        auto const ret_arg = f->getArg(0ul);
        BOOST_TEST_REQUIRE(ret_arg->getType()->isPointerTy());
        BOOST_TEST(ret_arg->getType()->getPointerElementType()->isIntegerTy(32));
        BOOST_TEST(ret_arg->hasStructRetAttr());
        BOOST_TEST_REQUIRE(f->getEntryBlock().size() == 7ul);
        auto it = f->getEntryBlock().begin();
        auto const* const gep1 = &*it++;
        auto const* const store1 = &*it++;
        auto const* const gep2 = &*it++;
        auto const* const store2 = &*it++;
        auto const* const gep3 = &*it++;
        auto const* const store3 = &*it++;
        BOOST_TEST(is_gep(gep1, is_i32_type, exactly<llvm::Value>(ret_arg), signed_constant(0)));
        BOOST_TEST(is_gep(gep2, is_i32_type, exactly<llvm::Value>(ret_arg), signed_constant(1)));
        BOOST_TEST(is_gep(gep3, is_i32_type, exactly<llvm::Value>(ret_arg), signed_constant(2)));
        BOOST_TEST(is_store(store1, signed_constant(1), exactly<llvm::Value>(gep1)));
        BOOST_TEST(is_store(store2, signed_constant(2), exactly<llvm::Value>(gep2)));
        BOOST_TEST(is_store(store3, signed_constant(3), exactly<llvm::Value>(gep3)));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)

BOOST_AUTO_TEST_SUITE_END()
