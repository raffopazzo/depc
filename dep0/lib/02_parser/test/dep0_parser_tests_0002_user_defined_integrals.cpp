#define BOOST_TEST_MODULE dep0_parser_tests_0002_user_defined_integrals
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0002_user_defined_integrals, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_000.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 10ul);
    BOOST_TEST(pass_result->func_defs.size() == 16ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "hours_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        23));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[1],
        "minutes_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        59));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[2],
        "seconds_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        59));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[3],
        "millis_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_16,
        999));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[4],
        "nanos_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_32,
        999999999));
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
        127));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[7],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        1));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[8],
        "signal_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        15));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[9],
        "key_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_64,
        std::nullopt));
}
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0002_user_defined_integrals/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0002_user_defined_integrals/pass_002.depc")); }

BOOST_AUTO_TEST_CASE(parse_error_000) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_000.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_001) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_001.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_002) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_002.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_003) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_003.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_004) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_004.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_005) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_005.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_006) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_006.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_007) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_007.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_008) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_008.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_009) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_009.depc")); }
BOOST_AUTO_TEST_CASE(parse_error_010) { BOOST_TEST(fail("0002_user_defined_integrals/parse_error_010.depc")); }

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(pass("0002_user_defined_integrals/typecheck_error_000.depc")); }

BOOST_AUTO_TEST_SUITE_END()
