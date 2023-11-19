#define BOOST_TEST_MODULE dep0_typecheck_tests_0005_pi_types
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0005_pi_types, TypecheckTestsFixture)

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
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("id"), i32, var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "h");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("id"), int_(), var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "apply");
        BOOST_TEST(is_type_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "apply_f");
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            BOOST_TEST(is_expr_of(expr.properties.sort.get(), is_type_i32));
            BOOST_TEST_REQUIRE(is_app_of(expr, var("apply"), i32, var("f"), numeric_constant("-1")));
            auto const& app = std::get<dep0::typecheck::expr_t::app_t>(expr.value);
            BOOST_TEST_REQUIRE(app.args.size() == 3ul);
            BOOST_TEST(is_expr_of(app.args[0ul].properties.sort.get(), is_typename));
            BOOST_TEST(is_expr_of(app.args[1ul].properties.sort.get(), [] (dep0::typecheck::expr_t const& t)
            {
                return is_arr_of(t, std::tuple{term_binder("x", is_type_i32)}, is_type_i32);
            }));
            BOOST_TEST(is_expr_of(app.args[2ul].properties.sort.get(), is_type_i32));
            return boost::test_tools::predicate_result(true);
        }));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "apply_g");
        BOOST_TEST(is_type_u32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), u32, var("g"), numeric_constant("1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "apply_h");
        BOOST_TEST(is_type_var(f.value.ret_type.get(), "int"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), int_(), var("h"), numeric_constant("1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "apply_0");
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), numeric_constant("0"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "discard_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("f", [] (dep0::typecheck::expr_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("u"), term_binder(type_var("u"))}, type_var("u"));
        })));
        BOOST_TEST(is_arg(f.value.args[2ul], term_binder("x", type_var("t"))));
        BOOST_TEST(is_type_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "discard_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("f", [] (dep0::typecheck::expr_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("t"), term_binder(type_var("t"))}, type_var("t"));
        })));
        BOOST_TEST(is_arg(f.value.args[2ul], term_binder("x", type_var("t"))));
        BOOST_TEST(is_type_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[14ul];
        BOOST_TEST(f.name == "discard_id_v1");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_u32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("discard_v1"), u32, var("id"), is_zero);
        }));
    }
    {
        auto const& f = pass_result->func_defs[15ul];
        BOOST_TEST(f.name == "discard_id_v2");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_u32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("discard_v2"), u32, var("id"), is_zero);
        }));
    }
    {
        auto const& f = pass_result->func_defs[17ul];
        BOOST_TEST(f.name == "multi_f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 5ul);
        BOOST_TEST(is_arg(f.value.args[0ul], type_binder("t")));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("f", [] (dep0::typecheck::expr_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("u"), term_binder(type_var("u"))}, type_var("u"));
        })));
        BOOST_TEST(is_arg(f.value.args[2ul], term_binder("x", type_var("t"))));
        BOOST_TEST(is_arg(f.value.args[3ul], term_binder("y", type_var("t"))));
        BOOST_TEST(is_arg(f.value.args[4ul], term_binder("z", type_var("t"))));
        BOOST_TEST(is_type_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const if_1 = std::get_if<dep0::typecheck::stmt_t::if_else_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_1);
        BOOST_TEST(is_app_of(if_1->cond, var("f"), bool_, boolean_constant(true)));
        BOOST_TEST_REQUIRE(if_1->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_1->false_branch->stmts.size() == 1ul);
        auto const if_2 = std::get_if<dep0::typecheck::stmt_t::if_else_t>(&if_1->false_branch->stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_2);
        BOOST_TEST(is_app_of(if_2->cond, var("int_to_bool"), [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), int_(), is_zero);
        }));
        BOOST_TEST_REQUIRE(if_2->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_2->false_branch->stmts.size() == 1ul);
        BOOST_TEST(is_return_of(if_2->false_branch->stmts[0], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), type_expr_of<dep0::typecheck::properties_t>(type_var("t")), var("z"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[19ul];
        BOOST_TEST(f.name == "get_id");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_arr_of(f.value.ret_type.get(), std::tuple{type_binder("t"), term_binder(type_var("t"))}, type_var("t")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("id")));
    }
    {
        auto const& f = pass_result->func_defs[22ul];
        BOOST_TEST(f.name == "apply_id_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("x", is_type_i32)));
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
    }
    {
        auto const& f = pass_result->func_defs[23ul];
        BOOST_TEST(f.name == "apply_id_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("x", is_type_i32)));
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0005_pi_types/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);

    {
        auto const t = std::get_if<dep0::typecheck::type_def_t::integer_t>(&pass_result->type_defs[0].value);
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
        BOOST_TEST(is_type_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& g = pass_result->func_defs[1ul];
        BOOST_TEST(g.name == "g");
        BOOST_TEST(g.value.args.empty());
        BOOST_TEST(is_type_var(g.value.ret_type.get(), "int"));
        BOOST_TEST_REQUIRE(g.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(g.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), int_(), i32, is_zero);
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
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("x", is_type_i32)));
        BOOST_TEST(is_arg(f.value.args[1ul], term_binder("y", is_type_i32)));
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("y"), numeric_constant("2"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.empty());
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
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
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], numeric_constant("0")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), type_expr_of<dep0::typecheck::properties_t>(is_type_u32));
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
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], numeric_constant("0")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("h", [] (dep0::typecheck::expr_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder(std::nullopt)}, is_type_i32);
        })));
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("h"), type_expr_of<dep0::typecheck::properties_t>(is_type_u32));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
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
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], numeric_constant("0")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("h", [] (dep0::typecheck::expr_t const& t)
        {
            return is_arr_of(t, std::tuple{term_binder(is_type_i32)}, is_type_i32);
        })));
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("h"), numeric_constant("1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
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
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], numeric_constant("0")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], term_binder("f", [] (dep0::typecheck::expr_t const& t)
        {
            return is_arr_of(t, std::tuple<>{}, is_type_i32);
        })));
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("f"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            return is_app_of(expr, var("g"), var("zero"));
        }));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(fail("0005_pi_types/typecheck_error_000.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between numeric constant and `t`");
    BOOST_TEST(fail_result->location.has_value());
    BOOST_TEST_REQUIRE(fail_result->tgt.has_value());
    std::ostringstream tgt;
    pretty_print(tgt, fail_result->tgt.value());
    BOOST_TEST(tgt.str() == "t");
}
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(fail("0005_pi_types/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(fail("0005_pi_types/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST(fail("0005_pi_types/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST(fail("0005_pi_types/typecheck_error_004.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_005) { BOOST_TEST(fail("0005_pi_types/typecheck_error_005.depc")); }

BOOST_AUTO_TEST_SUITE_END()
