#define BOOST_TEST_MODULE dep0_parser_tests_0016_impossible
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;
using enum dep0::ast::qty_t;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0016_impossible, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0016_impossible/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "absurd");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_typename, "t"));
        BOOST_TEST(is_arg(f->value.args[1], true_t_of(constant(false)), std::nullopt, zero));
        BOOST_TEST(is_var(f->value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_impossible(f->value.body.stmts[0]));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], true_t_of(constant(false)), "p", zero));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(var("absurd"), is_i32, var("p"))));
    }
}

BOOST_AUTO_TEST_SUITE_END()
