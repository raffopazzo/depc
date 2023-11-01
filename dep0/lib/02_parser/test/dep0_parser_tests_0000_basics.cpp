#define BOOST_TEST_MODULE dep0_parser_tests_0000_basics
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0000_basics, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(test_0000)
{
    BOOST_TEST_REQUIRE(pass("0000_basics/test_0000.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 0ul);
    BOOST_TEST(pass_result->func_defs.size() == 0ul);
    BOOST_TEST(pass_result->properties.line == 9);
    BOOST_TEST(pass_result->properties.col == 1);
    BOOST_TEST(pass_result->properties.txt == "");
}
BOOST_AUTO_TEST_CASE(test_0001)
{
    std::string const source = "i32_t main()\n{\n    return 0;\n}";
    std::string const file_source = source + '\n';
    BOOST_TEST_REQUIRE(pass("0000_basics/test_0001.depc"));
    BOOST_TEST(pass_result->properties.line == 1);
    BOOST_TEST(pass_result->properties.col == 1);
    BOOST_TEST(pass_result->properties.txt == file_source);

    BOOST_TEST(pass_result->type_defs.size() == 0ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.properties.line == 1);
    BOOST_TEST(f.properties.col == 1);
    BOOST_TEST(f.properties.txt == source);
    BOOST_TEST(f.value.ret_type.properties.line == 1);
    BOOST_TEST(f.value.ret_type.properties.col == 1);
    BOOST_TEST(f.value.ret_type.properties.txt == "i32_t");
    BOOST_TEST(std::holds_alternative<dep0::parser::type_t::i32_t>(f.value.ret_type.value));
    BOOST_TEST(f.name == "main");
}

BOOST_AUTO_TEST_CASE(test_0002)
{
    BOOST_TEST_REQUIRE(pass("0000_basics/test_0002.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 0ul);
    BOOST_TEST(pass_result->func_defs.size() == 1ul);
}

BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("0000_basics/test_0003.depc")); }
BOOST_AUTO_TEST_CASE(test_0004) { BOOST_TEST(pass("0000_basics/test_0004.depc")); }
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("0000_basics/test_0005.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(pass("0000_basics/test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(pass("0000_basics/test_0007.depc")); }
BOOST_AUTO_TEST_CASE(test_0008) { BOOST_TEST(pass("0000_basics/test_0008.depc")); }
BOOST_AUTO_TEST_CASE(test_0009) { BOOST_TEST(pass("0000_basics/test_0009.depc")); }
BOOST_AUTO_TEST_CASE(test_0010) { BOOST_TEST(pass("0000_basics/test_0010.depc")); }
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(pass("0000_basics/test_0011.depc")); }
BOOST_AUTO_TEST_CASE(test_0012) { BOOST_TEST(pass("0000_basics/test_0012.depc")); }
BOOST_AUTO_TEST_CASE(test_0013) { BOOST_TEST(pass("0000_basics/test_0013.depc")); }
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(pass("0000_basics/test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(pass("0000_basics/test_0015.depc")); }
BOOST_AUTO_TEST_CASE(test_0016) { BOOST_TEST(pass("0000_basics/test_0016.depc")); }
BOOST_AUTO_TEST_CASE(test_0017) { BOOST_TEST(pass("0000_basics/test_0017.depc")); }
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(pass("0000_basics/test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(pass("0000_basics/test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(pass("0000_basics/test_0020.depc")); }
BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(pass("0000_basics/test_0021.depc")); }
BOOST_AUTO_TEST_CASE(test_0022) { BOOST_TEST(pass("0000_basics/test_0022.depc")); }
BOOST_AUTO_TEST_CASE(test_0023) { BOOST_TEST(pass("0000_basics/test_0023.depc")); }

BOOST_AUTO_TEST_SUITE_END()
