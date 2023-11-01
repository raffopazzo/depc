#define BOOST_TEST_MODULE dep0_parser_tests_0002_user_defined_integrals
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0002_user_defined_integrals, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(test_0152)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/test_0152.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 10ul);
    BOOST_TEST(pass_result->func_defs.size() == 16ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "hours_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        "23"));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[1],
        "minutes_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        "59"));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[2],
        "seconds_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        "59"));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[3],
        "millis_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_16,
        "999"));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[4],
        "nanos_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_32,
        "999'999'999"));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[5],
        "duration_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_64,
        std::nullopt));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[6],
        "ascii_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        "127"));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[7],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        "1"));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[8],
        "signal_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        "15"));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[9],
        "key_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_64,
        std::nullopt));
}
BOOST_AUTO_TEST_CASE(test_0153) { BOOST_TEST(pass("0002_user_defined_integrals/test_0153.depc")); }
BOOST_AUTO_TEST_CASE(test_0154) { BOOST_TEST(fail("0002_user_defined_integrals/test_0154.depc")); }
BOOST_AUTO_TEST_CASE(test_0155) { BOOST_TEST(fail("0002_user_defined_integrals/test_0155.depc")); }
BOOST_AUTO_TEST_CASE(test_0156) { BOOST_TEST(fail("0002_user_defined_integrals/test_0156.depc")); }
BOOST_AUTO_TEST_CASE(test_0157) { BOOST_TEST(fail("0002_user_defined_integrals/test_0157.depc")); }
BOOST_AUTO_TEST_CASE(test_0158) { BOOST_TEST(fail("0002_user_defined_integrals/test_0158.depc")); }
BOOST_AUTO_TEST_CASE(test_0159) { BOOST_TEST(fail("0002_user_defined_integrals/test_0159.depc")); }
BOOST_AUTO_TEST_CASE(test_0160) { BOOST_TEST(fail("0002_user_defined_integrals/test_0160.depc")); }
BOOST_AUTO_TEST_CASE(test_0161) { BOOST_TEST(fail("0002_user_defined_integrals/test_0161.depc")); }
BOOST_AUTO_TEST_CASE(test_0162) { BOOST_TEST(fail("0002_user_defined_integrals/test_0162.depc")); }
BOOST_AUTO_TEST_CASE(test_0163) { BOOST_TEST(pass("0002_user_defined_integrals/test_0163.depc")); }
BOOST_AUTO_TEST_CASE(test_0164) { BOOST_TEST(pass("0002_user_defined_integrals/test_0164.depc")); }
BOOST_AUTO_TEST_CASE(test_0165) { BOOST_TEST(fail("0002_user_defined_integrals/test_0165.depc")); }
BOOST_AUTO_TEST_CASE(test_0166) { BOOST_TEST(fail("0002_user_defined_integrals/test_0166.depc")); }

BOOST_AUTO_TEST_SUITE_END()
