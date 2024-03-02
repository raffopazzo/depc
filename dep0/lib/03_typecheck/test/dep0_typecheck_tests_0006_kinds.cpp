#define BOOST_TEST_MODULE dep0_typecheck_tests_0006_kinds
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0006_kinds, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "type_identity");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{typename_("t")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("t")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "id");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{typename_("t"), arg_of(var("t"), "x")}, var("t"))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f->value.args[1ul], var("t"), "x"));
        BOOST_TEST(is_var(f->value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(global("id"), app_of(global("type_identity"), is_i32), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 4ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "type_identity");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{typename_("t")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("t")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "id");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{typename_("t"), arg_of(var("t"), "x")}, var("t"))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f->value.args[1ul], var("t"), "x"));
        BOOST_TEST(is_var(f->value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "apply_i32");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(pi_of(std::tuple{typename_("t")}, is_typename), "f")},
                    is_typename)));
        BOOST_TEST(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], pi_of(std::tuple{typename_("t")}, is_typename), "f"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("f"), is_i32)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(global("id"), app_of(global("apply_i32"), global("type_identity")), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 5ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "type_identity");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{typename_("t")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("t")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "id");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{typename_("t"), arg_of(var("t"), "x")}, var("t"))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f->value.args[1ul], var("t"), "x"));
        BOOST_TEST(is_var(f->value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("x")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "apply_i32");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(pi_of(std::tuple{typename_("t")}, is_typename), "f")},
                    is_typename)));
        BOOST_TEST(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], pi_of(std::tuple{typename_("t")}, is_typename), "f"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("f"), is_i32)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "get_type_identity");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(std::tuple{}, pi_of(std::tuple{typename_()}, is_typename))));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_pi_of(f->value.ret_type.get(), std::tuple{typename_()}, is_typename));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], global("type_identity")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(global("id"), app_of(global("apply_i32"), app_of(global("get_type_identity"))), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 10ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negate");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "plus_1");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32, "x")}, is_i32)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_i32, "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(var("x"), constant(1))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "int_or_bool");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "which")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_bool)}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "select");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(is_bool, "which")},
                    pi_of(
                        std::tuple{arg_of(app_of(global("int_or_bool"), var("which")))},
                        app_of(global("int_or_bool"), var("which"))))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(
            is_pi_of(
                f->value.ret_type.get(),
                std::tuple{arg_of(app_of(global("int_or_bool"), var("which")))},
                app_of(global("int_or_bool"), var("which"))));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(global("plus_1"))},
                std::tuple{return_of(global("negate"))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "apply");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{
                        typename_("t"),
                        arg_of(pi_of(std::tuple{arg_of(var("t"))}, var("t")), "f"),
                        arg_of(var("t"), "x")},
                    var("t"))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_typename, "t"));
        BOOST_TEST(is_arg(f->value.args[1ul], pi_of(std::tuple{arg_of(var("t"))}, var("t")), "f"));
        BOOST_TEST(is_arg(f->value.args[2ul], var("t"), "x"));
        BOOST_TEST(is_var(f->value.ret_type.get(), "t"));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("f"), var("x"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    global("apply"),
                    app_of(global("int_or_bool"), constant(true)),
                    app_of(global("select"), constant(true)),
                    constant(1))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[6ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    global("apply"),
                    app_of(global("int_or_bool"), constant(false)),
                    app_of(global("select"), constant(false)),
                    constant(false))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[7ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "select2");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(is_bool, "which")},
                    pi_of(
                        std::tuple{arg_of(app_of(global("int_or_bool"), var("which")))},
                        app_of(global("int_or_bool"), var("which"))))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(
            is_pi_of(
                f->value.ret_type.get(),
                std::tuple{arg_of(app_of(global("int_or_bool"), var("which")))},
                app_of(global("int_or_bool"), var("which"))));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(global("plus_1"))}));
        BOOST_TEST(is_return_of(f->value.body.stmts[1ul], global("negate")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[8ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f2");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    global("apply"),
                    app_of(global("int_or_bool"), constant(true)),
                    app_of(global("select2"), constant(true)),
                    constant(1))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[9ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g2");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    global("apply"),
                    app_of(global("int_or_bool"), constant(false)),
                    app_of(global("select2"), constant(false)),
                    constant(false))));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_004.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 8ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "negate");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "x")}, is_bool)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "x"));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("x"),
                std::tuple{return_of(constant(false))},
                std::tuple{return_of(constant(true))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "int_or_bool");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "which")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_bool)}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "to_bool");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32)}, is_bool)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_i32, std::nullopt));
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(false)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "to_int");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool)}, is_i32)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, std::nullopt));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], constant(0)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "select");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(is_bool, "which")},
                    pi_of(
                        std::tuple{arg_of(app_of(global("int_or_bool"), var("which")))},
                        app_of(global("int_or_bool"), app_of(global("negate"), var("which")))))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(
            is_pi_of(
                f->value.ret_type.get(),
                std::tuple{arg_of(app_of(global("int_or_bool"), var("which")))},
                app_of(global("int_or_bool"), app_of(global("negate"), var("which")))));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(global("to_bool"))},
                std::tuple{return_of(global("to_int"))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "always_int");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{
                        arg_of(is_bool, "which"),
                        arg_of(
                            pi_of(
                                std::tuple{arg_of(app_of(global("int_or_bool"), var("which")))},
                                app_of(global("int_or_bool"), app_of(global("negate"), var("which")))),
                            "f"),
                        arg_of(app_of(global("int_or_bool"), var("which")), "x")},
                    is_i32)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 3ul);
        BOOST_TEST(is_arg(f->value.args[0ul], is_bool, "which"));
        BOOST_TEST(is_arg(
            f->value.args[1ul],
            pi_of(
                std::tuple{arg_of(app_of(global("int_or_bool"), var("which")))},
                app_of(global("int_or_bool"), app_of(global("negate"), var("which")))),
            "f"));
        BOOST_TEST(is_arg(f->value.args[2ul], app_of(global("int_or_bool"), var("which")), "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(var("x"))},
                std::tuple{return_of(app_of(var("f"), var("x")))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[6ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_from_int");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    global("always_int"),
                    constant(true),
                    app_of(global("select"), constant(true)),
                    constant(0))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[7ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_from_bool");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->value.body.stmts[0ul],
                app_of(
                    global("always_int"),
                    constant(false),
                    app_of(global("select"), constant(false)),
                    constant(false))));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_005.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 7ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "int_or_bool");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "which")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_bool)}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_of");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(is_bool, "which")},
                    app_of(global("int_or_bool"), var("which")))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), global("int_or_bool"), var("which")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(constant(0))},
                std::tuple{return_of(constant(false))}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_of_2");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(is_bool, "which")},
                    app_of(global("int_or_bool"), var("which")))));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(is_app_of(f->value.ret_type.get(), global("int_or_bool"), var("which")));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(constant(0))}));
        BOOST_TEST(is_return_of(f->value.body.stmts[1ul], constant(false)));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_int");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("zero_of"), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[4ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_int_2");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("zero_of_2"), constant(true))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[5ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_bool");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("zero_of"), constant(false))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[6ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "zero_bool_2");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_bool)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_bool(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("zero_of_2"), constant(false))));
    }
}

BOOST_AUTO_TEST_CASE(pass_006)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_006.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "type_id");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_typename, "t")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_typename, "t"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("t")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, app_of(global("type_id"), is_unit))));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_app_of(f->value.ret_type.get(), global("type_id"), is_unit));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, app_of(global("type_id"), is_unit))));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_app_of(f->value.ret_type.get(), global("type_id"), is_unit));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of_void(f->value.body.stmts[0ul]));
    }
}

BOOST_AUTO_TEST_CASE(pass_007)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_007.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 4ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "succ");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_i32, "x")}, is_i32)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_i32, "x"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], plus(var("x"), constant(1))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "type_id");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_typename, "t")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_typename, "t"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], var("t")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "apply_0");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{arg_of(app_of(global("type_id"), pi_of(std::tuple{arg_of(is_i32)}, is_i32)), "f")},
                    is_i32)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], app_of(global("type_id"), pi_of(std::tuple{arg_of(is_i32)}, is_i32)), "f"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(var("f"), constant(0))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "one");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_i32)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("apply_0"), global("succ"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_008)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_008.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_typename)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], is_i32));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(app_of(global("f")))}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], app_of(global("f")), std::nullopt));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], is_u64));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{
                        arg_of(pi_of(std::tuple{}, is_typename), "f"),
                        arg_of(app_of(global("g"), constant(0)))},
                    is_unit)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], pi_of(std::tuple{}, is_typename), "f"));
        BOOST_TEST(is_arg(f->value.args[1], app_of(global("g"), constant(0)), std::nullopt));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
}

BOOST_AUTO_TEST_CASE(pass_009)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_009.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 3ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "which")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_bool)}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{
                        arg_of(is_bool, "x"),
                        arg_of(app_of(global("f"), var("x")))},
                    is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "x"));
        BOOST_TEST(is_arg(f->value.args[1], app_of(global("f"), var("x")), std::nullopt));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], is_u64));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{
                        arg_of(pi_of(std::tuple{arg_of(is_bool)}, is_typename), "f"),
                        arg_of(app_of(global("g"), constant(true), constant(0)))},
                    is_unit)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], pi_of(std::tuple{arg_of(is_bool)}, is_typename), "f"));
        BOOST_TEST(is_arg(f->value.args[1], app_of(global("g"), constant(true), constant(0)), std::nullopt));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
}

BOOST_AUTO_TEST_CASE(pass_010)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_010.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 4ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "which")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_bool)}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(std::tuple{}, pi_of(std::tuple{arg_of(is_bool)}, is_typename))));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_pi_of(f->value.ret_type.get(), std::tuple{arg_of(is_bool)}, is_typename));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], global("f")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{
                        arg_of(pi_of(std::tuple{arg_of(is_bool)}, is_typename), "f"),
                        arg_of(app_of(app_of(global("g")), constant(true)))},
                    is_unit)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], pi_of(std::tuple{arg_of(is_bool)}, is_typename), "f"));
        BOOST_TEST(is_arg(f->value.args[1], app_of(app_of(global("g")), constant(true)), std::nullopt));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "z");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_unit)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_app_of(f->value.body.stmts[0ul], global("h"), global("f"), constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_011)
{
    BOOST_TEST_REQUIRE(pass("0006_kinds/pass_011.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 4ul);
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{arg_of(is_bool, "which")}, is_typename)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 1ul);
        BOOST_TEST(is_arg(f->value.args[0], is_bool, "which"));
        BOOST_TEST(is_typename(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                var("which"),
                std::tuple{return_of(is_i32)},
                std::tuple{return_of(is_bool)}));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "g");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(std::tuple{}, pi_of(std::tuple{arg_of(is_bool)}, is_typename))));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_pi_of(f->value.ret_type.get(), std::tuple{arg_of(is_bool)}, is_typename));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], global("f")));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[2ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "h");
        BOOST_TEST(
            is_expr_of(
                f->properties.sort.get(),
                pi_of(
                    std::tuple{
                        arg_of(pi_of(std::tuple{arg_of(is_bool)}, is_typename), "f"),
                        arg_of(app_of(app_of(global("g")), constant(true)))},
                    is_unit)));
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], pi_of(std::tuple{arg_of(is_bool)}, is_typename), "f"));
        BOOST_TEST(is_arg(f->value.args[1], app_of(app_of(global("g")), constant(true)), std::nullopt));
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST(f->value.body.stmts.size() == 0ul);
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[3ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "z");
        BOOST_TEST(is_expr_of(f->properties.sort.get(), pi_of(std::tuple{}, is_unit)));
        BOOST_TEST(f->value.args.size() == 0ul);
        BOOST_TEST(is_unit(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f->value.body.stmts[0ul], app_of(global("h"), global("f"), constant(0))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST_REQUIRE(fail("0006_kinds/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST_REQUIRE(fail("0006_kinds/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST_REQUIRE(fail("0006_kinds/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST_REQUIRE(fail("0006_kinds/typecheck_error_003.depc")); }


BOOST_AUTO_TEST_SUITE_END()
