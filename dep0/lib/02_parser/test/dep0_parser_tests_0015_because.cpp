#define BOOST_TEST_MODULE dep0_parser_tests_0015_because
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0015_because, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0015_because/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 5ul);
    {
        auto const f = std::get_if<dep0::parser::axiom_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "trans");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 5ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_u64, "a"));
        BOOST_TEST(is_arg(f->signature.args[1], is_u64, "b"));
        BOOST_TEST(is_arg(f->signature.args[2], is_u64, "c"));
        BOOST_TEST(is_arg(f->signature.args[3], true_t_of(lt(var("a"), var("b"))), std::nullopt));
        BOOST_TEST(is_arg(f->signature.args[4], true_t_of(lt(var("b"), var("c"))), std::nullopt));
        BOOST_TEST(is_true_t_of(f->signature.ret_type.get(), lt(var("a"), var("c"))));
    }
    {
        auto const f = std::get_if<dep0::parser::extern_decl_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "rand");
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(constant(2), var("n"))), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                because_of(
                    subscript_of(var("xs"), constant(0)),
                    app_of(var("trans"), constant(0), constant(2), var("n"), is_auto, is_auto))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 5ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "i", dep0::ast::qty_t::one));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "n", dep0::ast::qty_t::one));
        BOOST_TEST(is_arg(f->value.args[2], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_arg(f->value.args[3], true_t_of(lt(var("i"), constant(2))), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[4], true_t_of(lt(constant(2), var("n"))), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                because_of(
                    subscript_of(var("xs"), var("i")),
                    app_of(var("trans"), var("i"), constant(2), var("n"), is_auto, is_auto))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(constant(0), var("n"))), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                because_of(
                    subscript_of(var("xs"), constant(0)),
                    app_of(var("rand")))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0015_because/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 4ul);
    {
        auto const f = std::get_if<dep0::parser::extern_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "rand");
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                because_of(
                    constant(0),
                    because_of(constant(false), constant(true)))));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x", dep0::ast::qty_t::one));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                because_of(
                    var("x"),
                    because_of(constant(true), var("x")))));

    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0],
                because_of(
                    constant(0),
                    because_of(constant(true), app_of(var("rand"))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0015_because/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_because_of(f->value.ret_type.get(), is_i32, constant(true)));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(var("f1"))));
    }
}

BOOST_AUTO_TEST_SUITE_END()
