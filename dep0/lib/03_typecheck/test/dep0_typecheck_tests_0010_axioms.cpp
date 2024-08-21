#define BOOST_TEST_MODULE dep0_typecheck_tests_0010_axioms
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0010_axioms, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0010_axioms/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::axiom_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "not_gte_implies_lt");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 3ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->signature.args[1], is_u64, "n"));
        BOOST_TEST(is_arg(f->signature.args[2], true_t_of(not_of(gte(var("i"), var("n")))), std::nullopt));
        BOOST_TEST(is_true_t_of(f->signature.ret_type.get(), lt(var("i"), var("n"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                gte(var("i"), constant(3)),
                std::tuple{return_of(constant(-1))},
                std::tuple{return_of(subscript_of(var("xs"), var("i")))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0010_axioms/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::axiom_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "not_gte_implies_lt");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 3ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->signature.args[1], is_u64, "n"));
        BOOST_TEST(is_arg(f->signature.args[2], true_t_of(not_of(gte(var("i"), var("n")))), "p"));
        BOOST_TEST(is_true_t_of(f->signature.ret_type.get(), lt(var("i"), var("n"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 1ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                gte(var("i"), constant(3)),
                std::tuple{return_of(constant(-1))},
                std::tuple{return_of(subscript_of(var("xs"), var("i")))}));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0010_axioms/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->entries.size() == 2ul);
    {
        auto const f = std::get_if<dep0::typecheck::axiom_t>(&pass_result->entries[0ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "not_gte_implies_lt");
        BOOST_TEST_REQUIRE(f->signature.args.size() == 3ul);
        BOOST_TEST(is_arg(f->signature.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->signature.args[1], is_u64, "n"));
        BOOST_TEST(is_arg(f->signature.args[2], true_t_of(not_of(gte(var("i"), var("n")))), "p"));
        BOOST_TEST(is_true_t_of(f->signature.ret_type.get(), lt(var("i"), var("n"))));
    }
    {
        auto const f = std::get_if<dep0::typecheck::func_def_t>(&pass_result->entries[1ul]);
        BOOST_TEST_REQUIRE(f);
        BOOST_TEST(f->name == "f");
        BOOST_TEST_REQUIRE(f->value.args.size() == 2ul);
        BOOST_TEST(is_arg(f->value.args[0], is_u64, "i"));
        BOOST_TEST(is_arg(f->value.args[1], array_of(is_i32, constant(3)), "xs"));
        BOOST_TEST(is_i32(f->value.ret_type.get()));
        BOOST_TEST_REQUIRE(f->value.body.stmts.size() == 2ul);
        BOOST_TEST(
            is_if_else(
                f->value.body.stmts[0ul],
                gte(var("i"), constant(3)),
                std::tuple{return_of(constant(-1))}));
        BOOST_TEST(is_return_of(f->value.body.stmts[1ul], subscript_of(var("xs"), var("i"))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST_REQUIRE(pass("0010_axioms/pass_003.depc")); }

// BOOST_AUTO_TEST_CASE(typecheck_error_000) -- this test was removed
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(fail("0010_axioms/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(fail("0010_axioms/typecheck_error_002.depc")); }

BOOST_AUTO_TEST_SUITE_END()

