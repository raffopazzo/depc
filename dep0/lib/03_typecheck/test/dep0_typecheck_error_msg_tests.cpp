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

BOOST_AUTO_TEST_CASE(test_0002)
{
    auto const result = check(tt::context_t(), open("test_0002.depc"));
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expecting expression of type `int`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0004)
{
    auto const module = open("test_0004.depc");
    auto const result = check(tt::context_t(), module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expression of type `int` does not typecheck with expected type `unit_t`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0008)
{
    auto const module = open("test_0008.depc");
    auto const result = check(tt::context_t(), module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expression of type `bool` does not typecheck with expected type `int`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0009)
{
    auto const module = open("test_0009.depc");
    auto const result = check(tt::context_t(), module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expression of type `int` does not typecheck with expected type `bool`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0010)
{
    auto const module = open("test_0010.depc");
    auto const result = check(tt::context_t(), module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "In function `main` missing return statement");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0012)
{
    auto const module = open("test_0012.depc");
    auto const result = check(tt::context_t(), module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expression of type `bool` does not typecheck with expected type `int`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0013)
{
    auto const module = open("test_0013.depc");
    auto const result = check(tt::context_t(), module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "In function `main` missing return statement");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_SUITE_END()
