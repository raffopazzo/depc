#define BOOST_TEST_MODULE dep0_parser_tests_0006_kinds
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0006_kinds, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "type_identity");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_typename(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("t")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], var("t"), "x"));
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                app_of(var("id"), app_of(var("type_identity"), is_i32), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 4ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "type_identity");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_typename(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("t")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], var("t"), "x"));
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "apply_i32");
        BOOST_TEST(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], pi_of(std::tuple{typename_("t")}, is_typename), "f"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f"), is_i32)));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                app_of(var("id"), app_of(var("apply_i32"), var("type_identity")), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 5ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "type_identity");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_typename(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("t")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], var("t"), "x"));
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "apply_i32");
        BOOST_TEST(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], pi_of(std::tuple{typename_("t")}, is_typename), "f"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f"), is_i32)));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "get_type_identity");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_pi_of(f.value.ret_type.get(), std::tuple{typename_()}, is_typename));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("type_identity")));
    }
    {
        auto const& f = pass_result->func_defs[4ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                app_of(var("id"), app_of(var("apply_i32"), app_of(var("get_type_identity"))), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 10ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "negate");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_if_else(f.value.body.stmts[0ul], [] (dep0::parser::stmt_t::if_else_t const& if_)
        {
            BOOST_TEST(is_var(if_.cond, "x"));
            BOOST_TEST_REQUIRE(if_.true_branch.stmts.size() == 1ul);
            BOOST_TEST_REQUIRE(if_.false_branch.has_value());
            BOOST_TEST_REQUIRE(if_.false_branch->stmts.size() == 1ul);
            BOOST_TEST(is_return_of(if_.true_branch.stmts[0], constant(false)));
            BOOST_TEST(is_return_of(if_.false_branch->stmts[0], constant(true)));
            return boost::test_tools::predicate_result(true);
        }));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "plus_1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], plus(var("x"), constant(1))));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "int_or_bool");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_bool, "which"));
        BOOST_TEST(is_typename(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_if_else(f.value.body.stmts[0ul], [] (dep0::parser::stmt_t::if_else_t const& if_)
        {
            BOOST_TEST(is_var(if_.cond, "which"));
            BOOST_TEST_REQUIRE(if_.true_branch.stmts.size() == 1ul);
            BOOST_TEST_REQUIRE(if_.false_branch.has_value());
            BOOST_TEST_REQUIRE(if_.false_branch->stmts.size() == 1ul);
            BOOST_TEST(is_return_of(if_.true_branch.stmts[0], is_i32));
            BOOST_TEST(is_return_of(if_.false_branch->stmts[0], is_bool));
            return boost::test_tools::predicate_result(true);
        }));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "select");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_bool, "which"));
        BOOST_TEST(
            is_pi_of(
                f.value.ret_type.get(),
                std::tuple{arg_of(app_of(var("int_or_bool"), var("which")))},
                app_of(var("int_or_bool"), var("which"))));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_if_else(f.value.body.stmts[0ul], [] (dep0::parser::stmt_t::if_else_t const& if_)
        {
            BOOST_TEST(is_var(if_.cond, "which"));
            BOOST_TEST_REQUIRE(if_.true_branch.stmts.size() == 1ul);
            BOOST_TEST_REQUIRE(if_.false_branch.has_value());
            BOOST_TEST_REQUIRE(if_.false_branch->stmts.size() == 1ul);
            BOOST_TEST(is_return_of(if_.true_branch.stmts[0], var("plus_1")));
            BOOST_TEST(is_return_of(if_.false_branch->stmts[0], var("negate")));
            return boost::test_tools::predicate_result(true);
        }));
    }
    {
        auto const& f = pass_result->func_defs[4ul];
        BOOST_TEST(f.name == "apply");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(is_arg(f.value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f.value.args[1ul], pi_of(std::tuple{arg_of(var("t"))}, var("t")), "f"));
        BOOST_TEST(is_arg(f.value.args[2ul], var("t"), "x"));
        BOOST_TEST(is_var(f.value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("f"), var("x"))));
    }
    {
        auto const& f = pass_result->func_defs[5ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                app_of(
                    var("apply"),
                    app_of(var("int_or_bool"), constant(true)),
                    app_of(var("select"), constant(true)),
                    constant(1))));
    }
    {
        auto const& f = pass_result->func_defs[6ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                app_of(
                    var("apply"),
                    app_of(var("int_or_bool"), constant(false)),
                    app_of(var("select"), constant(false)),
                    constant(false))));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "select2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(is_arg(f.value.args[0], is_bool, "which"));
        BOOST_TEST(
            is_pi_of(
                f.value.ret_type.get(),
                std::tuple{arg_of(app_of(var("int_or_bool"), var("which")))},
                app_of(var("int_or_bool"), var("which"))));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 2ul);
        BOOST_TEST(is_if_else(f.value.body.stmts[0ul], [] (dep0::parser::stmt_t::if_else_t const& if_)
        {
            BOOST_TEST(is_var(if_.cond, "which"));
            BOOST_TEST_REQUIRE(if_.true_branch.stmts.size() == 1ul);
            BOOST_TEST(is_return_of(if_.true_branch.stmts[0], var("plus_1")));
            BOOST_TEST(not if_.false_branch.has_value());
            return boost::test_tools::predicate_result(true);
        }));
        BOOST_TEST(is_return_of(f.value.body.stmts[1ul], var("negate")));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "f2");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_i32(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                app_of(
                    var("apply"),
                    app_of(var("int_or_bool"), constant(true)),
                    app_of(var("select2"), constant(true)),
                    constant(1))));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "g2");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_bool(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f.value.body.stmts[0ul],
                app_of(
                    var("apply"),
                    app_of(var("int_or_bool"), constant(false)),
                    app_of(var("select2"), constant(false)),
                    constant(false))));
    }
}

BOOST_AUTO_TEST_SUITE_END()
