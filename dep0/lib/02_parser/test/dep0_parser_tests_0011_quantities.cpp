#define BOOST_TEST_MODULE dep0_parser_tests_0011_quantities
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0011_quantities, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0011_quantities/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_unit, "x", dep0::ast::qty_t::zero));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_unit, std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
    }
}

BOOST_AUTO_TEST_SUITE_END()

