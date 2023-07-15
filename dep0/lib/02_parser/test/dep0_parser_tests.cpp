#define BOOST_TEST_MODULE dep0_parser_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"

#include <filesystem>
#include <cstdlib>

namespace dep0
{

std::ostream& operator<<(std::ostream& os, expected<parser::module_t> const& x)
{
    if (x) return os << "Parsing ok";
    else return pretty_print(os << std::endl, x.error());
}

}

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
};

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000)
{
    auto const result = dep0::parser::parse(testfiles / "test_0000.depc");
    BOOST_TEST_REQUIRE(result);
    BOOST_TEST(result->properties.line == 9);
    BOOST_TEST(result->properties.col == 1);
    BOOST_TEST(result->properties.txt == "");
    BOOST_TEST(result->func_defs.empty());
}

BOOST_AUTO_TEST_CASE(test_0001)
{
    auto const result = dep0::parser::parse(testfiles / "test_0001.depc");
    std::string const source = "int main()\n{\n    return 0;\n}";
    std::string const file_source = source + '\n';
    BOOST_TEST_REQUIRE(result);
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
    BOOST_TEST(f.type.properties.txt == "int");
    BOOST_TEST(std::holds_alternative<dep0::parser::type_t::int_t>(f.type.value));
    BOOST_TEST(f.name == "main");
}

BOOST_AUTO_TEST_CASE(test_0002) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0002.depc")); }
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0003.depc")); }
BOOST_AUTO_TEST_CASE(test_0004) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0004.depc")); }
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0005.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0007.depc")); }
BOOST_AUTO_TEST_CASE(test_0008) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0008.depc")); }
BOOST_AUTO_TEST_CASE(test_0009) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0009.depc")); }
BOOST_AUTO_TEST_CASE(test_0010) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0010.depc")); }
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0011.depc")); }
BOOST_AUTO_TEST_CASE(test_0012) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0012.depc")); }
BOOST_AUTO_TEST_CASE(test_0013) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0013.depc")); }
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0015.depc")); }
BOOST_AUTO_TEST_CASE(test_0016) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0016.depc")); }
BOOST_AUTO_TEST_CASE(test_0017) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0017.depc")); }
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(dep0::parser::parse(testfiles / "test_0020.depc")); }

BOOST_AUTO_TEST_SUITE_END()
