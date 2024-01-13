#define BOOST_TEST_MODULE dep0_typecheck_tests_0007_arrays
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0007_arrays, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "values");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, array_of(is_i32, constant(3)))));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], init_list_of(constant(1), constant(2), constant(3))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "empty_of");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_typename, "t")}, array_of(var("t"), constant(0)))));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_typename, "t"));
        BOOST_TEST(is_array_of(f.value.ret_type.get(), var("t"), constant(0)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], init_list_of()));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "empty_of_int");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, array_of(is_i32, constant(0)))));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_i32, constant(0)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("empty_of"), is_i32)));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "empty_of_bool");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, array_of(is_bool, constant(0)))));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_bool, constant(0)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("empty_of"), is_bool)));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "discard");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(is_u64, "n"), arg_of(array_of(is_i32, var("n")))}, is_unit)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f.value.args[1], array_of(is_i32, var("n")), std::nullopt));
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST(f.value.body.stmts.size() == 0ul);
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "nothing");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_unit)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("discard"), constant(0), init_list_of())));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "values");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{}, array_of(is_i32, plus(constant(1), constant(1))))));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_i32, plus(constant(1), constant(1))));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], init_list_of(constant(1), constant(2))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "first");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_i32, constant(3)), "xs")}, is_i32)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], subscript_of(var("xs"), constant(0))));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "second");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_i32, constant(3)), "xs")}, is_i32)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], subscript_of(var("xs"), constant(1))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "third");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_i32, constant(3)), "xs")}, is_i32)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], subscript_of(var("xs"), constant(2))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{}, pi_of(std::tuple{arg_of(is_typename), arg_of(is_u64)}, is_typename))));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_pi_of(f.value.ret_type.get(), std::tuple{arg_of(is_typename), arg_of(is_u64)}, is_typename));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], is_array));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "sum");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{arg_of(array_of(is_i32, constant(3)), "xs")}, is_i32)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const xs_0 = subscript_of(var("xs"), constant(0));
        auto const xs_1 = subscript_of(var("xs"), constant(1));
        auto const xs_2 = subscript_of(var("xs"), constant(2));
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], plus(plus(xs_0, xs_1), xs_2)));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "fifteen");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST_REQUIRE(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const three = constant(3);
        auto const five = constant(5);
        auto const seven = constant(7);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("sum"), init_list_of(three, five, seven))));
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "unit");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_unit)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST(f.value.body.stmts.size() == 0ul);
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "three_units");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, array_of(is_unit, constant(3)))));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_unit, constant(3)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const unit = app_of(var("unit"));
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], init_list_of(unit, unit, unit)));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_008.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "matrix");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(is_u64, "cols"), arg_of(is_u64, "rows")},
                    is_typename)));
        BOOST_TEST(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_u64, "cols"));
        BOOST_TEST(is_arg(f.value.args[1ul], is_u64, "rows"));
        BOOST_TEST(is_typename(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], array_of(array_of(is_i32, var("cols")), var("rows"))));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(std::tuple{}, app_of(var("matrix"), constant(3), constant(2)))));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_app_of(f.value.ret_type.get(), var("matrix"), constant(3), constant(2)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                init_list_of(
                    init_list_of(constant(1), constant(2), constant(3)),
                    init_list_of(constant(4), constant(5), constant(6)))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(app_of(var("matrix"), constant(3), constant(2)), "m")},
                    is_i32)));
        BOOST_TEST(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], app_of(var("matrix"), constant(3), constant(2)), "m"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                plus(
                    subscript_of(subscript_of(var("m"), constant(0)), constant(1)),
                    subscript_of(subscript_of(var("m"), constant(1)), constant(2)))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST_REQUIRE(fail("0007_arrays/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST_REQUIRE(fail("0007_arrays/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST_REQUIRE(fail("0007_arrays/typecheck_error_002.depc")); }

BOOST_AUTO_TEST_SUITE_END()
