#define BOOST_TEST_MODULE dep0_llvmgen_tests_0011_quantities
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const sext = std::vector{llvm::Attribute::SExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0011_quantities, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0011_quantities/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0))));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const x = exactly(f->getArg(0));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), add_of(x, x)));
    }
    {
        auto const f = pass_result.value()->getFunction("g");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "y", sext), arg_of(is_i32, "z", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const y = exactly(f->getArg(0));
        auto const z = exactly(f->getArg(1));
        auto const f0 = exactly(pass_result.value()->getFunction("f0"));
        auto const f1 = exactly(pass_result.value()->getFunction("f1"));
        auto const f2 = exactly(pass_result.value()->getFunction("f2"));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(
                    add_of(
                        direct_call_of(f0),
                        add_of(direct_call_of(f0), direct_call_of(f1, call_arg(y, sext)))),
                    add_of(
                        add_of(direct_call_of(f0), direct_call_of(f1, call_arg(z, sext))),
                        direct_call_of(f2, call_arg(z, sext))))));
    }
}

BOOST_AUTO_TEST_SUITE_END()