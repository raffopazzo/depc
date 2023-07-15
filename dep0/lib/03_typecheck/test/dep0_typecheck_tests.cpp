#define BOOST_TEST_MODULE dep0_typecheck_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"

#include <filesystem>
#include <cstdlib>

namespace dep0
{

std::ostream& operator<<(std::ostream& os, expected<parser::module_t> const& x)
{
    if (x) return os << "Parsing ok";
    else return pretty_print(os << std::endl, x.error());
}

std::ostream& operator<<(std::ostream& os, expected<typecheck::module_t> const& x)
{
    if (x) return os << "Typechecking ok";
    else return pretty_print(os << std::endl, x.error());
}

}

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");

    dep0::parser::module_t open(std::filesystem::path const& file)
    {
        auto const module = dep0::parser::parse(testfiles / file);
        BOOST_TEST_REQUIRE(module);
        return *module;
    }
};

using namespace dep0::typecheck;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000)
{
    BOOST_TEST(check(tt::context_t(), open("test_0000.depc")));
}

BOOST_AUTO_TEST_CASE(test_0001)
{
    BOOST_TEST(check(tt::context_t(), open("test_0001.depc")));
}

BOOST_AUTO_TEST_CASE(test_0002)
{
    auto const result = check(tt::context_t(), open("test_0002.depc"));
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Expecting expression of type `int`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(check(tt::context_t(), open("test_0003.depc"))); }

BOOST_AUTO_TEST_CASE(test_0004)
{
    auto const module = open("test_0004.depc");
    auto const result = check(tt::context_t(), module);
    BOOST_TEST_REQUIRE(result.has_error());
    BOOST_TEST(result.error().error == "Type mismatch between numeric constant and `unit_t`");
    BOOST_TEST(result.error().location.has_value());
}

BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(check(tt::context_t(), open("test_0005.depc"))); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(check(tt::context_t(), open("test_0006.depc"))); }

BOOST_AUTO_TEST_SUITE_END()
