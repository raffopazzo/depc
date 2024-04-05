#define BOOST_TEST_MODULE dep0_parser_tests_0007_arrays
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0007_arrays, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "values");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(constant(1), constant(2), constant(3))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "empty_of");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_typename, "t"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), var("t"), constant(0)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "empty_of_int");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(0)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("empty_of"), is_i32)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "empty_of_bool");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_bool, constant(0)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("empty_of"), is_bool)));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "discard");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, var("n")), std::nullopt));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "nothing");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("discard"), constant(0), init_list_of())));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "values");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, plus(constant(1), constant(1))));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(constant(1), constant(2))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "first");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("xs"), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "second");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("xs"), constant(1))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "third");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("xs"), constant(2))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_pi_of(f->value.ret_type.get(), std::tuple{arg_of(is_typename), arg_of(is_u64)}, is_typename));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], is_array));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "sum");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        auto const xs_0 = subscript_of(var("xs"), constant(0));
        auto const xs_1 = subscript_of(var("xs"), constant(1));
        auto const xs_2 = subscript_of(var("xs"), constant(2));
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(plus(xs_0, xs_1), xs_2)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "fifteen");
        BOOST_TEST_REQUIRE(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        auto const three = constant(3);
        auto const five = constant(5);
        auto const seven = constant(7);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("sum"), init_list_of(three, five, seven))));
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "unit");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "three_units");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_unit, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        auto const unit = app_of(var("unit"));
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], init_list_of(unit, unit, unit)));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_008.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "matrix");
        BOOST_TEST(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_u64, "cols"));
        BOOST_TEST(is_arg(f->value.args[1ul], is_u64, "rows"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], array_of(array_of(is_i32, var("cols")), var("rows"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("matrix"), constant(3), constant(2)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(
                    init_list_of(constant(1), constant(2), constant(3)),
                    init_list_of(constant(4), constant(5), constant(6)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], app_of(var("matrix"), constant(3), constant(2)), "m"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    subscript_of(subscript_of(var("m"), constant(0)), constant(1)),
                    subscript_of(subscript_of(var("m"), constant(1)), constant(2)))));
    }
}

BOOST_AUTO_TEST_CASE(pass_009)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_009.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "sum");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    plus(
                        subscript_of(var("xs"), constant(0)),
                        subscript_of(var("xs"), constant(1))),
                    subscript_of(var("xs"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(array_of(is_i32, constant(3)), constant(4)), "m"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(var("sum"), subscript_of(var("m"), constant(2)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(array_of(is_i32, constant(3)), constant(4)), "m"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    plus(
                        plus(
                            app_of(var("sum"), subscript_of(var("m"), constant(0))),
                            app_of(var("sum"), subscript_of(var("m"), constant(1)))),
                        app_of(var("sum"), subscript_of(var("m"), constant(2)))),
                    app_of(var("sum"), subscript_of(var("m"), constant(3))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_010)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_010.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "values");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(constant(1), constant(2), constant(3))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "six");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    plus(
                        subscript_of(app_of(var("values")), constant(0)),
                        subscript_of(app_of(var("values")), constant(1))),
                    subscript_of(app_of(var("values")), constant(2)))));
    }
}

BOOST_AUTO_TEST_CASE(pass_011)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_011.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 5ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "transform_add");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_arg(f->value.args[1ul], array_of(pi_of(std::tuple{arg_of(is_i32)}, is_i32), constant(3)), "fs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    plus(
                        app_of(
                            subscript_of(var("fs"), constant(0)),
                            subscript_of(var("xs"), constant(0))),
                        app_of(
                            subscript_of(var("fs"), constant(1)),
                            subscript_of(var("xs"), constant(1)))),
                    app_of(
                        subscript_of(var("fs"), constant(2)),
                        subscript_of(var("xs"), constant(2))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "identity");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "plus_one");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(var("x"), constant(1))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "plus_two");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(var("x"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "eighteen");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    var("transform_add"),
                    init_list_of(constant(3), constant(5), constant(7)),
                    init_list_of(var("identity"), var("plus_one"), var("plus_two")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_012)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_012.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 5ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(1)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negate");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "select");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_pi_of(f->value.ret_type.get(), std::tuple{}, is_i32));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(var("f"))},
                std::tuple{return_of(var("g"))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "choose");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), pi_of(std::tuple{}, is_i32), constant(2)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(
                    app_of(var("select"), var("which")),
                    app_of(var("select"), app_of(var("negate"), var("which"))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_013)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_013.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "signed_or_unsigned");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "signed"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("signed"),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_u32)}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "first");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "signed"));
        BOOST_TEST(
            is_arg(
                f->value.args[1ul],
                array_of(
                    app_of(var("signed_or_unsigned"), var("signed")),
                    constant(3)),
                "xs"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("signed_or_unsigned"), var("signed")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("xs"), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "minus_one");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    var("first"),
                    constant(true),
                    init_list_of(constant(-1), constant(-2), constant(-3)))));
    }
}

BOOST_AUTO_TEST_CASE(pass_014)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_014.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 5ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zeros");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(constant(0), constant(0), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zeros");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(constant(0), constant(0), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "passthrough");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("zeros"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "values");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(app_of(var("zeros")))},
                std::tuple{return_of(init_list_of(constant(1), constant(2), constant(3)))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "select");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_arg(f->value.args[1ul], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(app_of(var("zeros")))},
                std::tuple{return_of(var("xs"))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "xs_or_ys");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_arg(f->value.args[1ul], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_arg(f->value.args[2ul], array_of(is_i32, constant(3)), "ys"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(var("xs"))},
                std::tuple{return_of(var("ys"))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_015)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_015.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "xs_or_ys");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_arg(f->value.args[1ul], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[2ul], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_arg(f->value.args[3ul], array_of(is_i32, var("n")), "ys"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, var("n")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(var("xs"))},
                std::tuple{return_of(var("ys"))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "xs_or_ys_2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_arg(f->value.args[1ul], array_of(is_i32, constant(2)), "xs"));
        BOOST_TEST(is_arg(f->value.args[2ul], array_of(is_i32, constant(2)), "ys"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(2)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(var("xs_or_ys"), var("which"), constant(2), var("xs"), var("ys"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_016)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_016.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "last");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("xs"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "count_3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "start_from_zero"));
        BOOST_TEST(is_array_of(f->value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("start_from_zero"),
                std::tuple{return_of(init_list_of(constant(0), constant(1), constant(2)))},
                std::tuple{return_of(init_list_of(constant(1), constant(2), constant(3)))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "two_or_three");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "start_from_zero"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                app_of(
                    var("last"),
                    app_of(
                        var("count_3"),
                        var("start_from_zero")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_017)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_017.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    auto const two = constant(2);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_i32, two), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    var("f"),
                    init_list_of(
                        subscript_of(var("xs"), constant(1)),
                        subscript_of(var("xs"), constant(0))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(array_of(is_i32, two), two), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    var("g"),
                    init_list_of(
                        subscript_of(var("xs"), constant(1)),
                        subscript_of(var("xs"), constant(0))))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(array_of(array_of(is_i32, two), two), two), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    var("h"),
                    init_list_of(
                        subscript_of(var("xs"), constant(1)),
                        subscript_of(var("xs"), constant(0))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_018)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_018.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 8ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "sq_matrix");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_u64, "n"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], array_of(array_of(is_i32, var("n")), var("n"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "id_matrix");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("sq_matrix"), constant(2)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(
                    init_list_of(constant(1), constant(0)),
                    init_list_of(constant(0), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "anti_id_matrix");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("sq_matrix"), constant(2)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                init_list_of(
                    init_list_of(constant(0), constant(1)),
                    init_list_of(constant(1), constant(0)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "select_matrix");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("sq_matrix"), constant(2)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(app_of(var("id_matrix")))},
                std::tuple{return_of(app_of(var("anti_id_matrix")))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "trace");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], app_of(var("sq_matrix"), constant(2)), "m"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    subscript_of(subscript_of(var("m"), constant(0)), constant(0)),
                    subscript_of(subscript_of(var("m"), constant(1)), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "anti_trace");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], app_of(var("sq_matrix"), constant(2)), "m"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                plus(
                    subscript_of(subscript_of(var("m"), constant(1)), constant(0)),
                    subscript_of(subscript_of(var("m"), constant(0)), constant(1)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "select_fn");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_pi_of(f->value.ret_type.get(), std::tuple{arg_of(app_of(var("sq_matrix"), constant(2)))}, is_i32));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(var("trace"))},
                std::tuple{return_of(var("anti_trace"))}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "test");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    app_of(var("select_fn"), var("which")),
                    app_of(var("select_matrix"), var("which")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_019)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_019.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "get");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[1ul], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                lt(var("i"), constant(3)),
                std::tuple{return_of(subscript_of(var("xs"), var("i")))},
                std::tuple{return_of(constant(0))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_020)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_020.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "third");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], true_t_of(lt(constant(2), var("n"))), "p"));
        BOOST_TEST(is_arg(f->value.args[2], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], subscript_of(var("xs"), constant(2))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "third2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], true_t_of(lt(constant(2), var("n"))), std::nullopt));
        BOOST_TEST(is_arg(f->value.args[2], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], subscript_of(var("xs"), constant(2))));
    }
}

BOOST_AUTO_TEST_CASE(pass_021)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_021.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(var("i"), var("n"))), std::nullopt));
        BOOST_TEST(is_arg(f->value.args[3], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], subscript_of(var("xs"), var("i"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(var("i"), var("n"))), "p"));
        BOOST_TEST(is_arg(f->value.args[3], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(var("f"), var("n"), var("i"), var("p"), var("xs"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(2)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                app_of(var("f"), constant(2), constant(0), init_list_of(), var("xs"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "invalid_length");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], array_of(is_i32, constant(-1)), "xs"));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_001)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/typecheck_error_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "invalid_size_type");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "n"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, var("n")), std::nullopt));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_002)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/typecheck_error_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "fourth");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("xs"), constant(3))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_003)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/typecheck_error_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "signed_or_unsigned");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "signed"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("signed"),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_u32)}));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "first");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "signed"));
        BOOST_TEST(
            is_arg(
                f->value.args[1ul],
                array_of(
                    app_of(var("signed_or_unsigned"), var("signed")),
                    constant(3)),
                "xs"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), var("signed_or_unsigned"), var("signed")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], subscript_of(var("xs"), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "error");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_u32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    var("first"),
                    constant(false),
                    init_list_of(constant(-1), constant(2), constant(3)))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_004)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/typecheck_error_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(var("i"), var("n"))), std::nullopt));
        BOOST_TEST(is_arg(f->value.args[3], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], subscript_of(var("xs"), var("i"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                app_of(var("f"), var("n"), constant(0), init_list_of(), var("xs"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_005)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/typecheck_error_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(var("i"), var("n"))), std::nullopt));
        BOOST_TEST(is_arg(f->value.args[3], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], subscript_of(var("xs"), var("i"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(is_i32, constant(2)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                app_of(var("f"), constant(2), constant(0), init_list_of(constant(0)), var("xs"))));
    }
}

BOOST_AUTO_TEST_SUITE_END()
