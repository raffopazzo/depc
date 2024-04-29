#define BOOST_TEST_MODULE dep0_hash_code_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/ast/hash_code.hpp"
#include "ast_tests_fixture.hpp"

namespace dep0::ast {

BOOST_FIXTURE_TEST_SUITE(dep0_hash_code_tests, AstTestsFixture)

BOOST_AUTO_TEST_CASE(basic_tests)
{
    BOOST_TEST(hash_code(typename_()) == hash_code(typename_()));
    BOOST_TEST(hash_code(typename_()) != hash_code(i32()));
    BOOST_TEST(hash_code(i32()) != hash_code(u32()));
    BOOST_TEST(hash_code(numeric_constant(1)) == hash_code(numeric_constant(1)));
    BOOST_TEST(hash_code(numeric_constant(1)) != hash_code(numeric_constant(2)));
    BOOST_TEST(hash_code(var("x")) == hash_code(var("x")));
    BOOST_TEST(hash_code(var("x")) != hash_code(var("y")));
}

BOOST_AUTO_TEST_CASE(pi_tests)
{
    BOOST_TEST(
        hash_code(pi({arg(i32(), "x")}, i32())) ==
        hash_code(pi({arg(i32(), "y")}, i32())));
    BOOST_TEST(
        hash_code(pi({arg(qty_t::zero, i32(), "x")}, i32())) !=
        hash_code(pi({arg(i32(), "y")}, i32())));
    BOOST_TEST(
        hash_code(pi({arg(i32(), "x")}, i32())) !=
        hash_code(pi({arg(i32(), "y")}, u32())));
    BOOST_TEST(
        hash_code(pi({arg(i32(), "x")}, i32())) ==
        hash_code(pi({arg(i32())}, i32())));
    BOOST_TEST(
        hash_code(pi({arg(i32())}, i32())) ==
        hash_code(pi({arg(i32(), "y")}, i32())));
    BOOST_TEST(
        hash_code(pi({arg(i32())}, i32())) ==
        hash_code(pi({arg(i32())}, i32())));
    BOOST_TEST(
        hash_code(pi({arg(i32())}, i32())) ==
        hash_code(pi({arg(i32())}, i32())));

    BOOST_TEST(
        hash_code(pi({arg(typename_(), "x")}, var("x"))) ==
        hash_code(pi({arg(typename_(), "y")}, var("y"))));
    BOOST_TEST(
        hash_code(pi({arg(typename_(), "x")}, var("x"))) !=
        hash_code(pi({arg(typename_(), "y")}, var("x"))));

    BOOST_TEST(
        hash_code(pi({arg(typename_(), "x")}, var("x"))) !=
        hash_code(pi({arg(typename_())}, var("x"))));

    BOOST_TEST(
        hash_code(pi({arg(typename_(), "x"), arg(var("x"))}, var("x"))) ==
        hash_code(pi({arg(typename_(), "y"), arg(var("y"))}, var("y"))));
    BOOST_TEST(
        hash_code(pi({arg(typename_(), "x"), arg(i32(), "x")}, i32())) ==
        hash_code(pi({arg(typename_()), arg(i32())}, i32())));
}

BOOST_AUTO_TEST_CASE(abs_tests)
{
    BOOST_TEST(
        hash_code(abs({arg(i32(), "x")}, i32(), body(return_(var("x"))))) ==
        hash_code(abs({arg(i32(), "x")}, i32(), body(return_(var("x"))))));
    BOOST_TEST(
        hash_code(abs({arg(qty_t::zero, i32(), "x")}, i32(), body(return_(var("x"))))) !=
        hash_code(abs({arg(i32(), "x")}, i32(), body(return_(var("x"))))));
    BOOST_TEST(
        hash_code(abs({arg(i32(), "x")}, i32(), body(return_(var("x"))))) ==
        hash_code(abs({arg(i32(), "y")}, i32(), body(return_(var("y"))))));
    BOOST_TEST(
        hash_code(abs({arg(i32(), "x")}, i32(), body(return_(var("x"))))) !=
        hash_code(abs({arg(i32(), "x")}, i32(), body(return_(var("y"))))));
    BOOST_TEST(
        hash_code(abs({arg(i32(), "x")}, i32(), body(return_(var("y"))))) ==
        hash_code(abs({arg(i32())}, i32(), body(return_(var("y"))))));
    BOOST_TEST(
        hash_code(abs({arg(i32())}, i32(), body(return_(var("y"))))) ==
        hash_code(abs({arg(i32())}, i32(), body(return_(var("y"))))));
    BOOST_TEST(
        hash_code(abs({arg(i32(), "x")}, i32(), body(return_(var("x"))))) !=
        hash_code(abs({arg(i32())}, i32(), body(return_(var("x"))))));
    BOOST_TEST(
        hash_code(abs({arg(i32(), "x"), arg(i32(), "x")}, i32(), body(return_(var("y"))))) ==
        hash_code(abs({arg(i32()), arg(i32())}, i32(), body(return_(var("y"))))));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
