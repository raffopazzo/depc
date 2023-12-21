#define BOOST_TEST_MODULE dep0_llvmgen_tests_0000_basics
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0000_basics, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000) { BOOST_TEST(pass("0000_basics/pass_000.depc")); }
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0000_basics/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0000_basics/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0000_basics/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0000_basics/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0000_basics/pass_005.depc")); }
BOOST_AUTO_TEST_CASE(pass_006) { BOOST_TEST(pass("0000_basics/pass_006.depc")); }
BOOST_AUTO_TEST_CASE(pass_007) { BOOST_TEST(pass("0000_basics/pass_007.depc")); }
BOOST_AUTO_TEST_CASE(pass_008) { BOOST_TEST(pass("0000_basics/pass_008.depc")); }
BOOST_AUTO_TEST_CASE(pass_009) { BOOST_TEST(pass("0000_basics/pass_009.depc")); }
BOOST_AUTO_TEST_CASE(pass_010) { BOOST_TEST(pass("0000_basics/pass_010.depc")); }
BOOST_AUTO_TEST_CASE(pass_011) { BOOST_TEST(pass("0000_basics/pass_011.depc")); }
BOOST_AUTO_TEST_CASE(pass_012) { BOOST_TEST(pass("0000_basics/pass_012.depc")); }
BOOST_AUTO_TEST_CASE(pass_013) { BOOST_TEST(pass("0000_basics/pass_013.depc")); }

BOOST_AUTO_TEST_CASE(pass_014)
{
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_014.depc"));
    {
        auto const f = pass_result.value()->getFunction("unit");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(is_return_of_void(f->getEntryBlock().getTerminator()));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_001) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_002) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_003) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_004) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_005) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_006) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_007) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_008) doesn't type check
// BOOST_AUTO_TEST_CASE(typecheck_error_009) doesn't type check

BOOST_AUTO_TEST_SUITE_END()
