#define BOOST_TEST_MODULE dep0_typecheck_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"

#include <filesystem>
#include <cstdlib>

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");

    dep0::parser::module_t open(std::filesystem::path const& file)
    {
        auto const module = dep0::parser::parse(testfiles / file);
        BOOST_TEST_REQUIRE(module.has_value());
        return *module;
    }
};

using namespace dep0::typecheck;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests, Fixture)

BOOST_AUTO_TEST_CASE(error_with_context)
{
    tt::context_t ctx;
    auto const foo = tt::type_t::var(dep0::source_text::from_literal("foo"));
    auto const bar = tt::type_t::var(dep0::source_text::from_literal("bar"));
    std::ignore = ctx.add(tt::term_t::var_t(dep0::source_text::from_literal("z")), foo);
    std::ignore = ctx.add(tt::term_t::var_t(dep0::source_text::from_literal("y")), bar);
    auto err = dep0::typecheck::error_t::from_error(dep0::error_t{"Test"}, ctx);
    std::ostringstream out;
    pretty_print(out, err);
    std::string expected = R"(Test
In context:
y: bar
z: foo)";
    BOOST_TEST(out.str() == expected);
}

BOOST_AUTO_TEST_CASE(error_with_context_and_target_type)
{
    tt::context_t ctx;
    auto const foo = tt::type_t::var(dep0::source_text::from_literal("foo"));
    auto const bar = tt::type_t::var(dep0::source_text::from_literal("bar"));
    std::ignore = ctx.add(tt::term_t::var_t(dep0::source_text::from_literal("z")), foo);
    std::ignore = ctx.add(tt::term_t::var_t(dep0::source_text::from_literal("y")), bar);
    auto err = dep0::typecheck::error_t::from_error(dep0::error_t{"Test"}, ctx);
    err.tgt = tt::type_t::arr(bar, foo);
    std::ostringstream out;
    pretty_print(out, err);
    std::string expected = R"(Test
In context:
y: bar
z: foo
------------
(bar) -> foo)";
    BOOST_TEST(out.str() == expected);
}

BOOST_AUTO_TEST_CASE(test_0002)
{
    auto const result = check(open("test_0002.depc"));
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expecting expression of type `i32_t`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0004)
{
    auto const module = open("test_0004.depc");
    auto const result = check(module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Type mismatch between numeric constant and `unit_t`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0008)
{
    auto const module = open("test_0008.depc");
    auto const result = check(module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expression of type `bool` does not typecheck with expected type `i32_t`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0009)
{
    auto const module = open("test_0009.depc");
    auto const result = check(module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Type mismatch between numeric constant and `bool`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0010)
{
    auto const module = open("test_0010.depc");
    auto const result = check(module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "In function `main` missing return statement");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0012)
{
    auto const module = open("test_0012.depc");
    auto const result = check(module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expression of type `bool` does not typecheck with expected type `i32_t`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0013)
{
    auto const module = open("test_0013.depc");
    auto const result = check(module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "In function `main` missing return statement");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_SUITE_END()
