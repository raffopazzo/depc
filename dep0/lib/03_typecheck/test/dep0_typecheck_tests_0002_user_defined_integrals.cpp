#define BOOST_TEST_MODULE dep0_typecheck_tests_0002_user_defined_integrals
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0002_user_defined_integrals, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_000.depc"));
    BOOST_TEST(pass_result->entries.size() == 26ul);
}
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0002_user_defined_integrals/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0002_user_defined_integrals/pass_002.depc")); }

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 8ul);
    {
        auto const t = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "sign_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_8, 1));
    }
    {
        auto const t = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "hour_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8, 23));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negative");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_global(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(-1)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_global(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_sign");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_global(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(1)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_sign");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_global(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(app_of(global("max_sign")), constant(1))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_hour");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_global(f->value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(23)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_hour");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_global(f->value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(app_of(global("max_hour")), constant(1))));
    }
    // normalization should handle integer signed/unsigned wrapping
    BOOST_TEST(dep0::typecheck::beta_delta_normalize(*pass_result));
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(-1)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 8ul);
    {
        auto const t = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "sign_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_8, 1));
    }
    {
        auto const t = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "hour_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_8, 23));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2]);
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
                std::tuple{return_of(global("hour_t"))},
                std::tuple{return_of(global("sign_t"))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "one_of");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), global("which"), var("x")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(1))},
                std::tuple{return_of(constant(1))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "max_of");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), global("which"), var("x")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(23))},
                std::tuple{return_of(constant(1))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_of");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), global("which"), var("x")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    app_of(global("max_of"), var("x")),
                    app_of(global("one_of"), var("x")))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_hour");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_global(f->value.ret_type.get(), "hour_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("min_of"), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "min_sign");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_global(f->value.ret_type.get(), "sign_t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("min_of"), constant(false))));
    }
    // normalization should handle integer signed/unsigned wrapping
    // and compute the correct type for the numeric constant
    BOOST_TEST(dep0::typecheck::beta_delta_normalize(*pass_result));
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[6]);
        BOOST_TEST_REQUIRE(f);
        auto const& stmt = f->value.body.stmts[0ul];
        BOOST_TEST_REQUIRE(is_return_of(stmt, constant(0)));
        auto const ret = std::get<dep0::typecheck::stmt_t::return_t>(stmt.value);
        BOOST_TEST(is_global(std::get<dep0::typecheck::expr_t>(ret.expr->properties.sort.get()), "hour_t"));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[7]);
        BOOST_TEST_REQUIRE(f);
        auto const& stmt = f->value.body.stmts[0ul];
        BOOST_TEST_REQUIRE(is_return_of(stmt, constant(-1)));
        auto const ret = std::get<dep0::typecheck::stmt_t::return_t>(stmt.value);
        BOOST_TEST(is_global(std::get<dep0::typecheck::expr_t>(ret.expr->properties.sort.get()), "sign_t"));
    }
}

// BOOST_AUTO_TEST_CASE(parse_error_000)
// BOOST_AUTO_TEST_CASE(parse_error_001)
// BOOST_AUTO_TEST_CASE(parse_error_002)
// BOOST_AUTO_TEST_CASE(parse_error_003)
// BOOST_AUTO_TEST_CASE(parse_error_004)
// BOOST_AUTO_TEST_CASE(parse_error_005)
// BOOST_AUTO_TEST_CASE(parse_error_006)
// BOOST_AUTO_TEST_CASE(parse_error_007)
// BOOST_AUTO_TEST_CASE(parse_error_008)
// BOOST_AUTO_TEST_CASE(parse_error_009)
// BOOST_AUTO_TEST_CASE(parse_error_010)

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_004.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_005) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_005.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_006) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_006.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_007) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_007.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_008) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_008.depc")); }

BOOST_AUTO_TEST_SUITE_END()
