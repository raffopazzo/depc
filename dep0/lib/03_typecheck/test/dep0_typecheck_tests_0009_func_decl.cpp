#define BOOST_TEST_MODULE dep0_typecheck_tests_0009_func_decl
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0009_func_decl, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 9ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32)}, is_bool)));
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f3");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32), arg_of(is_u32)}, is_bool)));
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_arg(f->signature.args[1ul], is_u32, std::nullopt));
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g1");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_typename)));
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_typename(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g2");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32)}, is_typename)));
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_typename(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g3");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32), arg_of(is_u32)}, is_typename)));
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_arg(f->signature.args[1ul], is_u32, std::nullopt));
        BOOST_TEST(is_typename(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[6ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h1");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, app_of(global("g1")))));
        BOOST_TEST(f->signature.args.size() == 0ul);
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), global("g1")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[7ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h2");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(is_i32, "x")},
                    app_of(global("g2"), var("x")))));
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, "x"));
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), global("g2"), var("x")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[8ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h3");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{
                        arg_of(is_i32, "x"),
                        arg_of(is_u32, "y")},
                app_of(global("g3"), var("x"), var("y")))));
        BOOST_TEST_REQUIRE(f->signature.args.size() == 2ul);
        BOOST_TEST(is_arg(f->signature.args[0ul], is_i32, "x"));
        BOOST_TEST(is_arg(f->signature.args[1ul], is_u32, "y"));
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), global("g3"), var("x"), var("y")));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0009_func_decl/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[0]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32)}, is_bool)));
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_i32, std::nullopt));
        BOOST_TEST(is_bool(f->signature.ret_type.get()));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32, "x")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0],
                app_of(global("f"), var("x")),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_bool)}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_decl_t>(&pass_result->entries[2]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(std::tuple{arg_of(is_i32, "x")}, app_of(global("g"), var("x")))));
        BOOST_TEST_REQUIRE(f->signature.args.size() == 1ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_i32, "x"));
        BOOST_TEST(is_app_of(f->signature.ret_type.get(), global("g"), var("x")));
    }
}

BOOST_AUTO_TEST_SUITE_END()
