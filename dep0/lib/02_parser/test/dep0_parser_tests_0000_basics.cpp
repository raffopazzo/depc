#define BOOST_TEST_MODULE dep0_parser_tests_0000_basics
#include <boost/test/unit_test.hpp>

#include "parser_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests_0000_basics, ParserTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_000.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 0ul);
    BOOST_TEST(pass_result->func_defs.size() == 0ul);
    BOOST_TEST(pass_result->properties.line == 9);
    BOOST_TEST(pass_result->properties.col == 1);
    BOOST_TEST(pass_result->properties.txt == "");
}
BOOST_AUTO_TEST_CASE(pass_001)
{
    std::string const source = "i32_t main()\n{\n    return 0;\n}";
    std::string const file_source = source + '\n';
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_001.depc"));
    BOOST_TEST(pass_result->properties.line == 1);
    BOOST_TEST(pass_result->properties.col == 1);
    BOOST_TEST(pass_result->properties.txt == file_source);

    BOOST_TEST(pass_result->type_defs.size() == 0ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.properties.line == 1);
    BOOST_TEST(f.properties.col == 1);
    BOOST_TEST(f.properties.txt == source);
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST(f.value.ret_type.get().properties.line == 1);
    BOOST_TEST(f.value.ret_type.get().properties.col == 1);
    BOOST_TEST(f.value.ret_type.get().properties.txt == "i32_t");
    BOOST_TEST(f.name == "main");
}

BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0000_basics/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0000_basics/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0000_basics/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0000_basics/pass_005.depc")); }
BOOST_AUTO_TEST_CASE(pass_006) { BOOST_TEST(pass("0000_basics/pass_006.depc")); }
BOOST_AUTO_TEST_CASE(pass_007) { BOOST_TEST(pass("0000_basics/pass_007.depc")); }
BOOST_AUTO_TEST_CASE(pass_008) { BOOST_TEST(pass("0000_basics/pass_008.depc")); }
BOOST_AUTO_TEST_CASE(pass_009) { BOOST_TEST(pass("0000_basics/pass_009.depc")); }
BOOST_AUTO_TEST_CASE(pass_010) { BOOST_TEST(pass("0000_basics/pass_010.depc")); }
BOOST_AUTO_TEST_CASE(pass_011) { BOOST_TEST(pass("0000_basics/pass_011.depc")); }
BOOST_AUTO_TEST_CASE(pass_012) { BOOST_TEST(pass("0000_basics/pass_012.depc")); }
BOOST_AUTO_TEST_CASE(pass_013) { BOOST_TEST(pass("0000_basics/pass_013.depc")); }

BOOST_AUTO_TEST_CASE(pass_014)
{
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_014.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "unit");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST(f.value.body.stmts.size() == 0ul);
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("unit"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(pass("0000_basics/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(pass("0000_basics/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(pass("0000_basics/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST(pass("0000_basics/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST(pass("0000_basics/typecheck_error_004.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_005) { BOOST_TEST(pass("0000_basics/typecheck_error_005.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_006) { BOOST_TEST(pass("0000_basics/typecheck_error_006.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_007) { BOOST_TEST(pass("0000_basics/typecheck_error_007.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_008) { BOOST_TEST(pass("0000_basics/typecheck_error_008.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_009) { BOOST_TEST(pass("0000_basics/typecheck_error_009.depc")); }

BOOST_AUTO_TEST_SUITE_END()
