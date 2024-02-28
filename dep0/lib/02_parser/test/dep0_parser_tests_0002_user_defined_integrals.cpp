#define BOOST_TEST_MODULE dep0_parser_tests_0002_user_defined_integrals
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0002_user_defined_integrals, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 10ul);
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

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 2ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        1));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[1],
        "hour_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        23));
    BOOST_TEST(pass_result->func_defs.size() == 6ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "negative");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_var(f.value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], constant(-1)));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_var(f.value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], constant(0)));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "max_sign");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_var(f.value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], constant(1)));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "min_sign");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_var(f.value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], plus(app_of(var("max_sign")), constant(1))));
    }
    {
        auto const& f = pass_result->func_defs[4ul];
        BOOST_TEST(f.name == "max_hour");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_var(f.value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], constant(23)));
    }
    {
        auto const& f = pass_result->func_defs[5ul];
        BOOST_TEST(f.name == "min_hour");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_var(f.value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], plus(app_of(var("max_hour")), constant(1))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 2ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        1));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[1],
        "hour_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        23));
    BOOST_TEST(pass_result->func_defs.size() == 6ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "which");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_typename(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f.value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(var("hour_t"))},
                std::tuple{return_of(var("sign_t"))}));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "one_of");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f.value.ret_type.get(), var("which"), var("x")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f.value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(1))},
                std::tuple{return_of(constant(1))}));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "max_of");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f.value.ret_type.get(), var("which"), var("x")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f.value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(23))},
                std::tuple{return_of(constant(1))}));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "min_of");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f.value.ret_type.get(), var("which"), var("x")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                plus(
                    app_of(var("max_of"), var("x")),
                    app_of(var("one_of"), var("x")))));
    }
    {
        auto const& f = pass_result->func_defs[4ul];
        BOOST_TEST(f.name == "min_hour");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_var(f.value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("min_of"), constant(true))));
    }
    {
        auto const& f = pass_result->func_defs[5ul];
        BOOST_TEST(f.name == "min_sign");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_var(f.value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("min_of"), constant(false))));
    }
}

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

BOOST_AUTO_TEST_CASE(typecheck_error_001)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        128));
}

BOOST_AUTO_TEST_CASE(typecheck_error_002)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "unsign_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        256));
}

BOOST_AUTO_TEST_CASE(typecheck_error_003)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_16,
        32768));
}

BOOST_AUTO_TEST_CASE(typecheck_error_004)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "unsign_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_16,
        65536));
}

BOOST_AUTO_TEST_CASE(typecheck_error_005)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_32,
        2147483648ul));
}

BOOST_AUTO_TEST_CASE(typecheck_error_006)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "unsign_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_32,
        4294967296ul));
}

BOOST_AUTO_TEST_CASE(typecheck_error_007)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_64,
        9223372036854775808ul));
}

BOOST_AUTO_TEST_CASE(typecheck_error_008)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_008.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0],
        "unsign_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_64,
        boost::multiprecision::cpp_int("18446744073709551616")));
}

BOOST_AUTO_TEST_SUITE_END()
