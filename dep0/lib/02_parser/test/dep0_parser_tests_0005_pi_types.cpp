#define BOOST_TEST_MODULE dep0_parser_tests_0005_pi_types
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0005_pi_types, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 24ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("x", type_var("t"))));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("id"), i32, var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "h");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("id"), var("int"), var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "apply");
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "apply_f");
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), i32, var("f"), numeric_constant("-1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "apply_g");
        BOOST_TEST(is_type_u32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), u32, var("g"), numeric_constant("1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "apply_h");
        BOOST_TEST(is_type_var(f.value.ret_type, "int"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), var("int"), var("h"), numeric_constant("1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "apply_0");
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), numeric_constant("0"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "discard_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("f", [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("u"), term_binder(type_var("u"))}, type_var("u"));
        })));
        BOOST_TEST(is_arg(f.value.args[2ul], term_binder("x", type_var("t"))));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "discard_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("f", [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("t"), term_binder(type_var("t"))}, type_var("t"));
        })));
        BOOST_TEST(is_arg(f.value.args[2ul], term_binder("x", type_var("t"))));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[14ul];
        BOOST_TEST(f.name == "discard_id_v1");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_u32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("discard_v1"), u32, var("id"), is_zero);
        }));
    }
    {
        auto const& f = pass_result->func_defs[15ul];
        BOOST_TEST(f.name == "discard_id_v2");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_u32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("discard_v2"), u32, var("id"), is_zero);
        }));
    }
    {
        auto const& f = pass_result->func_defs[17ul];
        BOOST_TEST(f.name == "multi_f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 5ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("f", [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("u"), term_binder(type_var("u"))}, type_var("u"));
        })));
        BOOST_TEST(is_arg(f.value.args[2ul], term_binder("x",type_var("t"))));
        BOOST_TEST(is_arg(f.value.args[3ul], term_binder("y",type_var("t"))));
        BOOST_TEST(is_arg(f.value.args[4ul], term_binder("z",type_var("t"))));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const if_1 = std::get_if<dep0::parser::stmt_t::if_else_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_1);
        BOOST_TEST(is_app_of(if_1->cond, var("f"), bool_, boolean_constant(true)));
        BOOST_TEST_REQUIRE(if_1->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_1->false_branch->stmts.size() == 1ul);
        auto const if_2 = std::get_if<dep0::parser::stmt_t::if_else_t>(&if_1->false_branch->stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_2);
        BOOST_TEST(is_app_of(if_2->cond, var("int_to_bool"), [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("int"), is_zero);
        }));
        BOOST_TEST_REQUIRE(if_2->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_2->false_branch->stmts.size() == 1ul);
        BOOST_TEST(is_return_of(if_2->false_branch->stmts[0], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("t"), var("z"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[19ul];
        BOOST_TEST(f.name == "get_id");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_arr_of(f.value.ret_type, std::tuple{type_binder("t"), term_binder(type_var("t"))}, type_var("t")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("id")));
    }
    {
        auto const& f = pass_result->func_defs[22ul];
        BOOST_TEST(f.name == "apply_id_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("x", is_type_i32)));
        BOOST_TEST(is_type_i32(f.value.ret_type));
    }
    {
        auto const& f = pass_result->func_defs[23ul];
        BOOST_TEST(f.name == "apply_id_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("x", is_type_i32)));
        BOOST_TEST(is_type_i32(f.value.ret_type));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);

    {
        auto const t = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[0].value);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(t->name == "int");
    }
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], type_binder("int")));
        BOOST_TEST(is_arg(f.value.args[2ul], term_binder("x", type_var("t"))));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& g = pass_result->func_defs[1ul];
        BOOST_TEST(g.name == "g");
        BOOST_TEST(g.value.args.empty());
        BOOST_TEST(is_type_var(g.value.ret_type, "int"));
        BOOST_TEST_REQUIRE(g.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(g.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("int"), i32, is_zero);
        }));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("x", is_type_i32)));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("y", is_type_i32)));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("x", is_type_i32)));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("y", is_type_i32)));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("y"), numeric_constant("2"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.empty());
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("g"), numeric_constant("1"), numeric_constant("0"));
        }));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder(std::nullopt)));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], numeric_constant("0")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), type_expr_of<dep0::parser::properties_t>(is_type_u32));
        }));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder(std::nullopt)));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], numeric_constant("0")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("h", [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder(std::nullopt)}, is_type_i32);
        })));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("h"), type_expr_of<dep0::parser::properties_t>(is_type_u32));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("g"), var("f"));
        }));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder(is_type_i32)));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], numeric_constant("0")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("h", [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{term_binder(is_type_i32)}, is_type_i32);
        })));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("h"), numeric_constant("1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("g"), var("f"));
        }));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], numeric_constant("0")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("f", [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple<>{}, is_type_i32);
        })));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("g"), var("zero"));
        }));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);

    auto const& f = pass_result->func_defs[0ul];
    BOOST_TEST(f.name == "succ");
    BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
    BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
    BOOST_TEST(is_arg(f.value.args[1ul], term_binder("x", type_var("t"))));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, var("x"), numeric_constant("1"));
    }));
}
BOOST_AUTO_TEST_CASE(typecheck_error_001)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/typecheck_error_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);

    auto const t = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[0].value);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(t->name == "t");

    auto const& f = pass_result->func_defs[0ul];
    BOOST_TEST(f.name == "succ");
    BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
    BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
    BOOST_TEST(is_arg(f.value.args[1ul], term_binder("x", type_var("t"))));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, var("x"), numeric_constant("1"));
    }));
}
BOOST_AUTO_TEST_CASE(typecheck_error_002)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/typecheck_error_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);

    {
        auto const t = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[0].value);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(t->name == "int");
    }
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("x", type_var("t"))));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("u")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("f", [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("int"), term_binder(type_var("int"))}, type_var("u"));
        })));
        BOOST_TEST(is_type_var(f.value.ret_type, "int"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], is_zero));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_var(f.value.ret_type, "int"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("g"), var("int"), var("id"));
        }));
    }
}
BOOST_AUTO_TEST_CASE(typecheck_error_003)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/typecheck_error_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0ul];
    BOOST_TEST(f.name == "f");
    BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
    BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
    BOOST_TEST(is_arg(f.value.args[1ul], type_binder("t")));
    BOOST_TEST(is_arg(f.value.args[2ul], term_binder("x", type_var("t"))));
    BOOST_TEST(is_type_var(f.value.ret_type, "t"));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
}

BOOST_AUTO_TEST_CASE(typecheck_error_004)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/typecheck_error_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0ul];
    BOOST_TEST(f.name == "f");
    BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
    BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
    BOOST_TEST(is_arg(f.value.args[1ul], term_binder("t", is_type_i32)));
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("t")));
}

BOOST_AUTO_TEST_CASE(typecheck_error_005)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/typecheck_error_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0ul];
    BOOST_TEST(f.name == "f");
    BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
    BOOST_TEST(is_arg(f.value.args[0ul], term_binder("t", is_type_i32)));
    BOOST_TEST(is_arg(f.value.args[1ul], type_binder("t")));
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("t")));
}

BOOST_AUTO_TEST_SUITE_END()
