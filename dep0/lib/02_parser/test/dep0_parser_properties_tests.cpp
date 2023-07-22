#define BOOST_TEST_MODULE dep0_parser_properties_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"

#include <filesystem>
#include <cstdlib>

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
};

BOOST_FIXTURE_TEST_SUITE(dep0_parser_properties_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000)
{
    auto const result = dep0::parser::parse(testfiles / "test_0000.depc");
    BOOST_TEST_REQUIRE(result.has_value());
    BOOST_TEST(result->properties.line == 9);
    BOOST_TEST(result->properties.col == 1);
    BOOST_TEST(result->properties.txt == "");
    BOOST_TEST(result->func_defs.empty());
}

BOOST_AUTO_TEST_CASE(test_0001)
{
    auto const result = dep0::parser::parse(testfiles / "test_0001.depc");
    std::string const source = "i32_t main()\n{\n    return 0;\n}";
    std::string const file_source = source + '\n';
    BOOST_TEST_REQUIRE(result.has_value());
    BOOST_TEST(result->properties.line == 1);
    BOOST_TEST(result->properties.col == 1);
    BOOST_TEST(result->properties.txt == file_source);
    BOOST_TEST_REQUIRE(result->func_defs.size() == 1ul);
    auto const& f = result->func_defs[0];
    BOOST_TEST(f.properties.line == 1);
    BOOST_TEST(f.properties.col == 1);
    BOOST_TEST(f.properties.txt == source);
    BOOST_TEST(f.type.properties.line == 1);
    BOOST_TEST(f.type.properties.col == 1);
    BOOST_TEST(f.type.properties.txt == "i32_t");
    BOOST_TEST(std::holds_alternative<dep0::parser::type_t::i32_t>(f.type.value));
    BOOST_TEST(f.name == "main");
}

BOOST_AUTO_TEST_SUITE_END()
