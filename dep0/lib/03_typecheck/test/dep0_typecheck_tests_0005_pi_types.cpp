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
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], var("t"), "x"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("id"), is_i32, var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "h");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("id"), var("int"), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "apply");
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f"), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "apply_f");
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
        {
            BOOST_TEST(is_expr_of(expr.properties.sort.get(), is_i32));
            BOOST_TEST_REQUIRE(is_app_of(expr, var("apply"), is_i32, var("f"), constant(-1)));
            auto const& app = std::get<dep0::typecheck::expr_t::app_t>(expr.value);
            BOOST_TEST_REQUIRE(app.args.size() == 3ul);
            BOOST_TEST(is_expr_of(app.args[0ul].properties.sort.get(), is_typename));
            BOOST_TEST(is_expr_of(app.args[1ul].properties.sort.get(), pi_of(std::tuple{arg_of(is_i32, "x")}, is_i32)));
            BOOST_TEST(is_expr_of(app.args[2ul].properties.sort.get(), is_i32));
            return boost::test_tools::predicate_result(true);
        }));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "apply_g");
        BOOST_TEST(is_u32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("apply"), is_u32, var("g"), constant(1))));
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "apply_h");
        BOOST_TEST(is_var(f.value.ret_type.get(), "int"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("apply"), var("int"), var("h"), constant(1))));
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "apply_0");
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("apply"), constant(0))));
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "discard_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], pi_of(std::tuple{typename_("u"), arg_of(var("u"))}, var("u")), "f"));
        BOOST_TEST(is_arg(f.value.args[2ul], var("t"), "x"));
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "discard_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], pi_of(std::tuple{typename_("t"), arg_of(var("t"))}, var("t")), "f"));
        BOOST_TEST(is_arg(f.value.args[2ul], var("t"), "x"));
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[14ul];
        BOOST_TEST(f.name == "discard_id_v1");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_u32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("discard_v1"), is_u32, var("id"), constant(0))));
    }
    {
        auto const& f = pass_result->func_defs[15ul];
        BOOST_TEST(f.name == "discard_id_v2");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_u32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("discard_v2"), is_u32, var("id"), constant(0))));
    }
    {
        auto const& f = pass_result->func_defs[17ul];
        BOOST_TEST(f.name == "multi_f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 5ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], pi_of(std::tuple{typename_("u"), arg_of(var("u"))}, var("u")), "f"));
        BOOST_TEST(is_arg(f.value.args[2ul], var("t"), "x"));
        BOOST_TEST(is_arg(f.value.args[3ul], var("t"), "y"));
        BOOST_TEST(is_arg(f.value.args[4ul], var("t"), "z"));
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const if_1 = std::get_if<dep0::typecheck::stmt_t::if_else_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_1);
        BOOST_TEST(is_app_of(if_1->cond, var("f"), is_bool, constant(true)));
        BOOST_TEST_REQUIRE(if_1->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_1->false_branch->stmts.size() == 1ul);
        auto const if_2 = std::get_if<dep0::typecheck::stmt_t::if_else_t>(&if_1->false_branch->stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_2);
        BOOST_TEST(is_app_of(if_2->cond, var("int_to_bool"), app_of(var("f"), var("int"), constant(0))));
        BOOST_TEST_REQUIRE(if_2->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_2->false_branch->stmts.size() == 1ul);
        BOOST_TEST(is_return_of(if_2->false_branch->stmts[0], app_of(var("f"), var("t"), var("z"))));
    }
    {
        auto const& f = pass_result->func_defs[19ul];
        BOOST_TEST(f.name == "get_id");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_pi_of(f.value.ret_type.get(), std::tuple{typename_("t"), arg_of(var("t"))}, var("t")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("id")));
    }
    {
        auto const& f = pass_result->func_defs[22ul];
        BOOST_TEST(f.name == "apply_id_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
    }
    {
        auto const& f = pass_result->func_defs[23ul];
        BOOST_TEST(f.name == "apply_id_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
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
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], is_typename, "int"));
        BOOST_TEST(is_arg(f.value.args[2ul], var("t"), "x"));
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& g = pass_result->func_defs[1ul];
        BOOST_TEST(g.name == "g");
        BOOST_TEST(g.value.args.empty());
        BOOST_TEST(is_var(g.value.ret_type.get(), "int"));
        BOOST_TEST_REQUIRE(g.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(g.value.body.stmts[0ul], app_of(var("f"), var("int"), is_i32, constant(0))));
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
        BOOST_TEST(is_arg(f.value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_arg(f.value.args[1ul], is_i32, "y"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_arg(f.value.args[1ul], is_i32, "y"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f"), var("y"), constant(2))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.empty());
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("g"), constant(1), constant(0))));
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
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, std::nullopt));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], constant(0)));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f"), is_u32)));
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
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, std::nullopt));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], constant(0)));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], pi_of(std::tuple{typename_()}, is_i32), "h"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("h"), is_u32)));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("g"), var("f"))));
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
        BOOST_TEST(is_arg(f.value.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], constant(0)));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], pi_of(std::tuple{arg_of(is_i32)}, is_i32), "h"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("h"), constant(1))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("g"), var("f"))));
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
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], constant(0)));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], pi_of(std::tuple<>{}, is_i32), "f"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f"))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "main");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("g"), var("zero"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(fail("0005_pi_types/typecheck_error_000.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between numeric constant and `t`");
    BOOST_TEST(fail_result->location.has_value());
    BOOST_TEST_REQUIRE(fail_result->context.has_value());
    std::ostringstream tgt;
    pretty_print(tgt, fail_result->context->second);
    BOOST_TEST(tgt.str() == "t");
}
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(fail("0005_pi_types/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(fail("0005_pi_types/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST(fail("0005_pi_types/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST(fail("0005_pi_types/typecheck_error_004.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_005) { BOOST_TEST(fail("0005_pi_types/typecheck_error_005.depc")); }

BOOST_AUTO_TEST_SUITE_END()
