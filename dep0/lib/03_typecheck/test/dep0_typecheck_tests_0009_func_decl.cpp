#define BOOST_TEST_MODULE dep0_typecheck_tests_0009_func_decl
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0009_func_decl, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_decls.size() == 9ul);
    {
        auto const& f = pass_result->func_decls[0ul];
        BOOST_TEST(f.name == "f1");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f.signature.args.size() == 0ul);
        BOOST_TEST(is_bool(f.signature.ret_type.get()));
    }
    {
        auto const& f = pass_result->func_decls[1ul];
        BOOST_TEST(f.name == "f2");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_i32)}, is_bool)));
        BOOST_TEST_REQUIRE(f.signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f.signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_bool(f.signature.ret_type.get()));
    }
    {
        auto const& f = pass_result->func_decls[2ul];
        BOOST_TEST(f.name == "f3");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_i32), arg_of(is_u32)}, is_bool)));
        BOOST_TEST_REQUIRE(f.signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f.signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_arg(f.signature.args[1ul], is_u32, std::nullopt));
        BOOST_TEST(is_bool(f.signature.ret_type.get()));
    }
    {
        auto const& f = pass_result->func_decls[3ul];
        BOOST_TEST(f.name == "g1");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_typename)));
        BOOST_TEST(f.signature.args.size() == 0ul);
        BOOST_TEST(is_typename(f.signature.ret_type.get()));
    }
    {
        auto const& f = pass_result->func_decls[4ul];
        BOOST_TEST(f.name == "g2");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_i32)}, is_typename)));
        BOOST_TEST_REQUIRE(f.signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f.signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_typename(f.signature.ret_type.get()));
    }
    {
        auto const& f = pass_result->func_decls[5ul];
        BOOST_TEST(f.name == "g3");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{arg_of(is_i32), arg_of(is_u32)}, is_typename)));
        BOOST_TEST_REQUIRE(f.signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f.signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_arg(f.signature.args[1ul], is_u32, std::nullopt));
        BOOST_TEST(is_typename(f.signature.ret_type.get()));
    }
    {
        auto const& f = pass_result->func_decls[6ul];
        BOOST_TEST(f.name == "h1");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, app_of(global("g1")))));
        BOOST_TEST(f.signature.args.size() == 0ul);
        BOOST_TEST(is_app_of(f.signature.ret_type.get(), global("g1")));
    }
    {
        auto const& f = pass_result->func_decls[7ul];
        BOOST_TEST(f.name == "h2");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(is_i32, "x")},
                    app_of(global("g2"), var("x")))));
        BOOST_TEST_REQUIRE(f.signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f.signature.args[0ul], is_i32, "x"));
        BOOST_TEST(is_app_of(f.signature.ret_type.get(), global("g2"), var("x")));
    }
    {
        auto const& f = pass_result->func_decls[8ul];
        BOOST_TEST(f.name == "h3");
        BOOST_TEST(
            is_expr_of(
                f.properties.sort.get(),
                pi_of(
                    std::tuple{
                        arg_of(is_i32, "x"),
                        arg_of(is_u32, "y")},
                app_of(global("g3"), var("x"), var("y")))));
        BOOST_TEST_REQUIRE(f.signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f.signature.args[0ul], is_i32, "x"));
        BOOST_TEST(is_arg(f.signature.args[1ul], is_u32, "y"));
        BOOST_TEST(is_app_of(f.signature.ret_type.get(), global("g3"), var("x"), var("y")));
    }
}

BOOST_AUTO_TEST_SUITE_END()
