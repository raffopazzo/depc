#define BOOST_TEST_MODULE dep0_typecheck_tests_0012_auto_expr
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0012_auto_expr, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(var("i"), var("n"))), std::nullopt, dep0::ast::qty_t::zero));
        BOOST_TEST(is_arg(f->value.args[3], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], subscript_of(var("xs"), var("i"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0],
                lt(constant(0), var("n")),
                std::tuple{
                    return_of(
                        app_of(
                            global("f"),
                            constant(0),
                            var("n"),
                            expr_of_type(is_unnamed_var, true_t_of(lt(constant(0), var("n")))),
                            var("xs")))},
                std::tuple{return_of(constant(0))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 4ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[1], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[2], true_t_of(lt(var("i"), var("n"))), std::nullopt));
        BOOST_TEST(is_arg(f->value.args[3], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], subscript_of(var("xs"), var("i"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "n"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, var("n")), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0],
                lt(constant(0), var("n")),
                std::tuple{
                    return_of(
                        app_of(
                            global("f"),
                            constant(0),
                            var("n"),
                            expr_of_type(init_list_of(), true_t_of(lt(constant(0), var("n")))),
                            var("xs")))},
                std::tuple{return_of(constant(0))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 6ul);
    {
        auto const t = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "zero_t", dep0::ast::sign_t::signed_v, dep0::ast::width_t::_32,  0));
    }
    {
        auto const t = std::get_if<dep0::typecheck::type_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(is_integer_def(*t, "uzero_t", dep0::ast::sign_t::unsigned_v, dep0::ast::width_t::_32,  0));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f1");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], global("zero_t"), std::nullopt));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g1");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("f1"), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], global("uzero_t"), std::nullopt));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g2");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("f2"), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], array_of(true_t_of(constant(false)), constant(0)), std::nullopt));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("f"), init_list_of())));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0012_auto_expr/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_unit, std::nullopt));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0], app_of(global("f"), init_list_of())));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(fail("0012_auto_expr/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(fail("0012_auto_expr/typecheck_error_001.depc")); }

BOOST_AUTO_TEST_SUITE_END()

