#define BOOST_TEST_MODULE dep0_parser_tests_0002_user_defined_integrals
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;
using enum dep0::ast::sign_t;
using enum dep0::ast::width_t;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0002_user_defined_integrals, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_000.depc"));
    // 10 type defs + 16 func defs
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 26ul);

    auto const hours    = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    auto const minutes  = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
    auto const seconds  = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[2]);
    auto const millis   = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[3]);
    auto const nanos    = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[4]);
    auto const duration = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[5]);
    auto const ascii    = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[13]);
    auto const sign     = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[15]);
    auto const signal   = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[19]);
    auto const key      = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[23]);

    BOOST_TEST_REQUIRE(hours);
    BOOST_TEST_REQUIRE(minutes);
    BOOST_TEST_REQUIRE(seconds);
    BOOST_TEST_REQUIRE(millis);
    BOOST_TEST_REQUIRE(nanos);
    BOOST_TEST_REQUIRE(duration);
    BOOST_TEST_REQUIRE(ascii);
    BOOST_TEST_REQUIRE(sign);
    BOOST_TEST_REQUIRE(signal);
    BOOST_TEST_REQUIRE(key);


    BOOST_TEST(is_integer_def(*hours,    "hours_t",    unsigned_v, _8,  23));
    BOOST_TEST(is_integer_def(*minutes,  "minutes_t",  unsigned_v, _8,  59));
    BOOST_TEST(is_integer_def(*seconds,  "seconds_t",  unsigned_v, _8,  59));
    BOOST_TEST(is_integer_def(*millis,   "millis_t",   unsigned_v, _16, 999));
    BOOST_TEST(is_integer_def(*nanos,    "nanos_t",    unsigned_v, _32, 999999999));
    BOOST_TEST(is_integer_def(*duration, "duration_t", signed_v,   _64, std::nullopt));
    BOOST_TEST(is_integer_def(*ascii,    "ascii_t",    unsigned_v, _8,  127));
    BOOST_TEST(is_integer_def(*sign,     "sign_t",     signed_v,   _8,  1));
    BOOST_TEST(is_integer_def(*signal,   "signal_t",   signed_v,   _8,  15));
    BOOST_TEST(is_integer_def(*key,      "key_t",      unsigned_v, _64, std::nullopt));
}
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0002_user_defined_integrals/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0002_user_defined_integrals/pass_002.depc")); }

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_003.depc"));
    // 2 type defs + 6 func defs
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 8ul);
    auto const sign = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    auto const hour = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
    BOOST_TEST_REQUIRE(sign);
    BOOST_TEST_REQUIRE(hour);
    BOOST_TEST(is_integer_def(*sign, "sign_t", signed_v,   _8,  1));
    BOOST_TEST(is_integer_def(*hour, "hour_t", unsigned_v, _8, 23));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negative");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(-1)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_sign");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(1)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_sign");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(app_of(var("max_sign")), constant(1))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_hour");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(23)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_hour");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(app_of(var("max_hour")), constant(1))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_004.depc"));
    // 2 type defs + 6 func defs
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 8ul);
    auto const sign = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    auto const hour = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[1]);
    BOOST_TEST_REQUIRE(sign);
    BOOST_TEST_REQUIRE(hour);
    BOOST_TEST(is_integer_def(*sign, "sign_t", signed_v,   _8,  1));
    BOOST_TEST(is_integer_def(*hour, "hour_t", unsigned_v, _8, 23));
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "which");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(var("hour_t"))},
                std::tuple{return_of(var("sign_t"))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "one_of");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("which"), var("x")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(1))},
                std::tuple{return_of(constant(1))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_of");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("which"), var("x")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(23))},
                std::tuple{return_of(constant(1))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_of");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("which"), var("x")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    app_of(var("max_of"), var("x")),
                    app_of(var("one_of"), var("x")))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_hour");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("min_of"), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_sign");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_var(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("min_of"), constant(false))));
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
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(is_integer_def(*t, "sign_t", signed_v, _8, 128));
}

BOOST_AUTO_TEST_CASE(typecheck_error_002)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(is_integer_def(*t, "unsign_t", unsigned_v, _8, 256));
}

BOOST_AUTO_TEST_CASE(typecheck_error_003)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(is_integer_def(*t, "sign_t", signed_v, _16, 32768));
}

BOOST_AUTO_TEST_CASE(typecheck_error_004)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(is_integer_def(*t, "unsign_t", unsigned_v, _16, 65536));
}

BOOST_AUTO_TEST_CASE(typecheck_error_005)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(is_integer_def(*t, "sign_t", signed_v, _32, 2147483648ul));
}

BOOST_AUTO_TEST_CASE(typecheck_error_006)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(is_integer_def(*t, "unsign_t", unsigned_v, _32, 4294967296ul));
}

BOOST_AUTO_TEST_CASE(typecheck_error_007)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(is_integer_def(*t, "sign_t", signed_v, _64, 9223372036854775808ul));
}

BOOST_AUTO_TEST_CASE(typecheck_error_008)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/typecheck_error_008.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    auto const t = std::get_if<dep0::parser::type_def_t>(&pass_result->entries[0]);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(is_integer_def(*t, "unsign_t", unsigned_v, _64, boost::multiprecision::cpp_int("18446744073709551616")));
}

BOOST_AUTO_TEST_SUITE_END()
