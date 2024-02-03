#define BOOST_TEST_MODULE dep0_typecheck_tests_0008_boolean_expr
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0008_boolean_expr, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 2ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 14ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0ul],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        1));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[1ul],
        "hours_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        23));
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "negate");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f.value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f0");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lt(constant(false), constant(true))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "f1");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lt(
                    lt(constant(true), constant(true)),
                    constant(true))));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "f2");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lt(
                    constant(true),
                    lt(constant(true), constant(true)))));
    }
    {
        auto const& f = pass_result->func_defs[4ul];
        BOOST_TEST(f.name == "f3");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lt(
                    app_of(var("negate"), lt(constant(true), app_of(var("f1")))),
                    app_of(var("negate"), lt(app_of(var("f1")), constant(true))))));
    }
    {
        auto const& f = pass_result->func_defs[5ul];
        BOOST_TEST(f.name == "f4");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_i32, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lt(constant(1), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[6ul];
        BOOST_TEST(f.name == "f5");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_u32, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lt(var("x"), constant(2))));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "f6");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(var("sign_t"), "x"), arg_of(var("sign_t"), "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f.value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lt(var("x"), var("y"))));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "f7");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_i32, "x"), arg_of(is_i32, "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f.value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lt(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "f8");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_u32, "x"), arg_of(is_u32, "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f.value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lt(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "f9");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(var("hours_t"), "x"), arg_of(var("hours_t"), "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f.value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lt(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "f10");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_bool, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lt(
                    lt(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "f11");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_bool, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lt(
                    subscript_of(var("xs"), constant(0)),
                    lt(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "f12");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_i32, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lt(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 2ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 14ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0ul],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        1));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[1ul],
        "hours_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        23));
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "negate");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f.value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f0");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gt(constant(true), constant(false))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "f1");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gt(gt(constant(true), constant(true)), constant(true))));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "f2");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gt(constant(true), gt(constant(true), constant(true)))));
    }
    {
        auto const& f = pass_result->func_defs[4ul];
        BOOST_TEST(f.name == "f3");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gt(
                    app_of(var("negate"), gt(constant(true), app_of(var("f1")))),
                    app_of(var("negate"), gt(app_of(var("f1")), constant(true))))));
    }
    {
        auto const& f = pass_result->func_defs[5ul];
        BOOST_TEST(f.name == "f4");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_i32, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gt(constant(1), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[6ul];
        BOOST_TEST(f.name == "f5");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_u32, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gt(var("x"), constant(2))));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "f6");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(var("sign_t"), "x"), arg_of(var("sign_t"), "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f.value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gt(var("x"), var("y"))));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "f7");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_i32, "x"), arg_of(is_i32, "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f.value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gt(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "f8");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_u32, "x"), arg_of(is_u32, "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f.value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gt(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "f9");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(var("hours_t"), "x"), arg_of(var("hours_t"), "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f.value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gt(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "f10");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_bool, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gt(
                    gt(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "f11");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_bool, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gt(
                    subscript_of(var("xs"), constant(0)),
                    gt(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "f12");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_i32, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gt(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 2ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 14ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0ul],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        1));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[1ul],
        "hours_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        23));
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "negate");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f.value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f0");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lte(constant(false), constant(true))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "f1");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lte(lte(constant(true), constant(true)), constant(true))));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "f2");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lte(constant(true), lte(constant(true), constant(true)))));
    }
    {
        auto const& f = pass_result->func_defs[4ul];
        BOOST_TEST(f.name == "f3");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lte(
                    app_of(var("negate"), lte(constant(true), app_of(var("f1")))),
                    app_of(var("negate"), lte(app_of(var("f1")), constant(true))))));
    }
    {
        auto const& f = pass_result->func_defs[5ul];
        BOOST_TEST(f.name == "f4");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_i32, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lte(constant(1), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[6ul];
        BOOST_TEST(f.name == "f5");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_u32, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lte(var("x"), constant(2))));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "f6");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(var("sign_t"), "x"), arg_of(var("sign_t"), "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f.value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lte(var("x"), var("y"))));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "f7");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_i32, "x"), arg_of(is_i32, "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f.value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lte(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "f8");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_u32, "x"), arg_of(is_u32, "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f.value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lte(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "f9");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(var("hours_t"), "x"), arg_of(var("hours_t"), "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f.value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], lte(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "f10");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_bool, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lte(
                    lte(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "f11");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_bool, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lte(
                    subscript_of(var("xs"), constant(0)),
                    lte(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "f12");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_i32, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                lte(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0008_boolean_expr/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 2ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 14ul);
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[0ul],
        "sign_t",
        dep0::ast::sign_t::signed_v,
        dep0::ast::width_t::_8,
        1));
    BOOST_TEST(is_integer_def(
        pass_result->type_defs[1ul],
        "hours_t",
        dep0::ast::sign_t::unsigned_v,
        dep0::ast::width_t::_8,
        23));
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "negate");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f.value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f0");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gte(constant(true), constant(false))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "f1");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gte(gte(constant(true), constant(true)), constant(true))));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "f2");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gte(constant(true), gte(constant(true), constant(true)))));
    }
    {
        auto const& f = pass_result->func_defs[4ul];
        BOOST_TEST(f.name == "f3");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gte(
                    app_of(var("negate"), gte(constant(true), app_of(var("f1")))),
                    app_of(var("negate"), gte(app_of(var("f1")), constant(true))))));
    }
    {
        auto const& f = pass_result->func_defs[5ul];
        BOOST_TEST(f.name == "f4");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_i32, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gte(constant(1), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[6ul];
        BOOST_TEST(f.name == "f5");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_u32, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u32, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gte(var("x"), constant(2))));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "f6");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(var("sign_t"), "x"), arg_of(var("sign_t"), "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], var("sign_t"), "x"));
        BOOST_TEST(is_arg(f.value.args[1], var("sign_t"), "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gte(var("x"), var("y"))));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "f7");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_i32, "x"), arg_of(is_i32, "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_i32, "x"));
        BOOST_TEST(is_arg(f.value.args[1], is_i32, "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gte(var("x"), plus(var("x"), var("y")))));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "f8");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_u32, "x"), arg_of(is_u32, "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u32, "x"));
        BOOST_TEST(is_arg(f.value.args[1], is_u32, "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gte(plus(var("x"), var("y")), plus(var("x"), var("y")))));
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "f9");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(var("hours_t"), "x"), arg_of(var("hours_t"), "y")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], var("hours_t"), "x"));
        BOOST_TEST(is_arg(f.value.args[1], var("hours_t"), "y"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], gte(plus(var("x"), var("y")), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "f10");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_bool, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gte(
                    gte(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "f11");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_bool, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_bool, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gte(
                    subscript_of(var("xs"), constant(0)),
                    gte(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "f12");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_i32, constant(3)), "xs")}, is_bool)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                gte(
                    plus(subscript_of(var("xs"), constant(0)), subscript_of(var("xs"), constant(1))),
                    plus(subscript_of(var("xs"), constant(1)), subscript_of(var("xs"), constant(2))))));
    }
}

BOOST_AUTO_TEST_SUITE_END()
