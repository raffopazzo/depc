#define BOOST_TEST_MODULE dep0_llvmgen_tests_0000_basics
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0000_basics, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(test_0000) { BOOST_TEST(pass("0000_basics/test_0000.depc")); }
BOOST_AUTO_TEST_CASE(test_0001) { BOOST_TEST(pass("0000_basics/test_0001.depc")); }
// BOOST_AUTO_TEST_CASE(test_0002) doesn't type check
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("0000_basics/test_0003.depc")); }
// BOOST_AUTO_TEST_CASE(test_0004) doesn't type check
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("0000_basics/test_0005.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(pass("0000_basics/test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(pass("0000_basics/test_0007.depc")); }
// BOOST_AUTO_TEST_CASE(test_0008) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0009) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0010) doesn't type check
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(pass("0000_basics/test_0011.depc")); }
// BOOST_AUTO_TEST_CASE(test_0012) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0013) doesn't type check
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(pass("0000_basics/test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(pass("0000_basics/test_0015.depc")); }
// BOOST_AUTO_TEST_CASE(test_0016) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0017) doesn't type check
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(pass("0000_basics/test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(pass("0000_basics/test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(pass("0000_basics/test_0020.depc")); }
BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(pass("0000_basics/test_0021.depc")); }
BOOST_AUTO_TEST_CASE(test_0022) { BOOST_TEST(pass("0000_basics/test_0022.depc")); }
// BOOST_AUTO_TEST_CASE(test_0023) doesn't type check

BOOST_AUTO_TEST_SUITE_END()
