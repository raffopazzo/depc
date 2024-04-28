#define BOOST_TEST_MODULE dep0_parser_tests_0012_auto_expr
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0012_auto_expr, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(var("i"), var("n"))), std::nullopt, dep0::ast::qty_t::zero));
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
            is_if_else(
                f->value.body.stmts[0],
                lt(constant(0), var("n")),
                std::tuple{return_of(app_of(var("f"), constant(0), var("n"), is_auto, var("xs")))},
                std::tuple{return_of(constant(0))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "n"));
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
            is_if_else(
                f->value.body.stmts[0],
                lt(constant(0), var("n")),
                std::tuple{return_of(app_of(var("f"), constant(0), var("n"), is_auto, var("xs")))},
                std::tuple{return_of(constant(0))}));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/typecheck_error_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "absurd");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], true_t_of(constant(false)), "p"));
        BOOST_TEST(is_true_t_of(f->value.ret_type.get(), var("x")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(var("absurd"), var("x"), var("p"))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "a"));
        BOOST_TEST(is_arg(f->value.args[1], is_i32, "b"));
        BOOST_TEST(is_true_t_of(f->value.ret_type.get(), lt(var("a"), var("b"))));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(var("absurd"), lt(var("a"), var("b")), is_auto)));
    }
}

BOOST_AUTO_TEST_SUITE_END()
