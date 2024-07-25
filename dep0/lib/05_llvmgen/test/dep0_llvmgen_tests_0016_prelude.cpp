#define BOOST_TEST_MODULE dep0_llvmgen_tests_0016_prelude
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0016_prelude, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000) { BOOST_TEST(pass("0016_prelude/pass_000.depc")); }
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0016_prelude/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0016_prelude/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0016_prelude/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0016_prelude/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0016_prelude/pass_005.depc")); }

// BOOST_AUTO_TEST_CASE(typecheck_000)

BOOST_AUTO_TEST_SUITE_END()
