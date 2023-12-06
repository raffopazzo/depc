#define BOOST_TEST_MODULE dep0_typecheck_tests_0007_arrays
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0007_arrays, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0007_arrays/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "values");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_array_of(f.value.ret_type.get(), is_i32, constant(3)));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], init_list_of(constant(1), constant(2), constant(3))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST_REQUIRE(fail("0007_arrays/typecheck_error_000.depc")); }

BOOST_AUTO_TEST_SUITE_END()
