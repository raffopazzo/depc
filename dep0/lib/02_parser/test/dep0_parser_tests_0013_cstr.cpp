/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_parser_tests_0013_cstr
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0013_cstr, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0013_cstr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 12ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f0");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal("")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal("0")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal("hello world")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello \"world\")")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f4");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello 'world')")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f5");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello\\world)")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[6ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f6");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello\nworld)")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[7ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f7");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello\tworld)")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[8ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f8");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello\vworld)")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[9ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f9");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello\bworld)")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[10ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f10");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello\fworld)")));
    }
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[11ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f11");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_cstr(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], string_literal(R"(hello\rworld)")));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0013_cstr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 1ul);
    {
        auto const f = std::get_if<dep0::parser::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_cstr, std::nullopt));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
    }
}

BOOST_AUTO_TEST_CASE(parse_error_000) { BOOST_TEST_REQUIRE(fail("0013_cstr/parse_error_000.depc")); }

BOOST_AUTO_TEST_SUITE_END()

