#define BOOST_TEST_MODULE dep0_typecheck_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"

#include <filesystem>
#include <cstdlib>

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");

    boost::test_tools::predicate_result pass(std::filesystem::path const file)
    {
        auto parse_result = dep0::parser::parse(testfiles / file);
        if (parse_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), parse_result.error());
            return res;
        }
        auto check_result = dep0::typecheck::check(dep0::typecheck::tt::context_t(), *parse_result);
        if (check_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), check_result.error());
            return res;
        }
        return true;
    }

    boost::test_tools::predicate_result fail(std::filesystem::path const file)
    {
        auto parse_result = dep0::parser::parse(testfiles / file);
        if (parse_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), parse_result.error());
            return res;
        }
        auto check_result = dep0::typecheck::check(dep0::typecheck::tt::context_t(), *parse_result);
        if (check_result.has_value())
        {
            auto res = boost::test_tools::predicate_result(false);
            res.message() << "Was expecting typecheck to fail but it succeeded";
            return res;
        }
        return true;
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000) { BOOST_TEST(pass("test_0000.depc")); }
BOOST_AUTO_TEST_CASE(test_0001) { BOOST_TEST(pass("test_0001.depc")); }
BOOST_AUTO_TEST_CASE(test_0002) { BOOST_TEST(fail("test_0002.depc")); }
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("test_0003.depc")); }
BOOST_AUTO_TEST_CASE(test_0004) { BOOST_TEST(fail("test_0004.depc")); }
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("test_0005.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(pass("test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(pass("test_0007.depc")); }
BOOST_AUTO_TEST_CASE(test_0008) { BOOST_TEST(fail("test_0008.depc")); }
BOOST_AUTO_TEST_CASE(test_0009) { BOOST_TEST(fail("test_0009.depc")); }
BOOST_AUTO_TEST_CASE(test_0010) { BOOST_TEST(fail("test_0010.depc")); }
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(pass("test_0011.depc")); }
BOOST_AUTO_TEST_CASE(test_0012) { BOOST_TEST(fail("test_0012.depc")); }
BOOST_AUTO_TEST_CASE(test_0013) { BOOST_TEST(fail("test_0013.depc")); }
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(pass("test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(pass("test_0015.depc")); }
BOOST_AUTO_TEST_CASE(test_0016) { BOOST_TEST(fail("test_0016.depc")); }
BOOST_AUTO_TEST_CASE(test_0017) { BOOST_TEST(fail("test_0017.depc")); }
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(pass("test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(pass("test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(pass("test_0020.depc")); }
BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(pass("test_0021.depc")); }
BOOST_AUTO_TEST_CASE(test_0022) { BOOST_TEST(pass("test_0022.depc")); }
BOOST_AUTO_TEST_CASE(test_0023) { BOOST_TEST(fail("test_0023.depc")); }

BOOST_AUTO_TEST_SUITE_END()
