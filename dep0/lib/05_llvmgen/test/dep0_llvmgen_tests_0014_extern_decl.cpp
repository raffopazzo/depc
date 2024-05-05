#define BOOST_TEST_MODULE dep0_llvmgen_tests_0014_extern_decl
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0014_extern_decl, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_000.depc"));
    auto const f0 = pass_result.value()->getFunction("f0");
    auto const f1 = pass_result.value()->getFunction("f1");
    auto const g = pass_result.value()->getFunction("g");
    BOOST_TEST_REQUIRE(is_function_of(f0, std::tuple{}, is_i32, sext));
    BOOST_TEST_REQUIRE(
        is_function_of(
            f1,
            std::tuple{
                arg_of(is_i32, std::nullopt, sext),
                arg_of(pointer_to(is_i8), std::nullopt, nonnull)},
            is_i32, sext));
    BOOST_TEST_REQUIRE(is_function_of(g, std::tuple{}, is_i32, sext));
    BOOST_TEST_REQUIRE(g->size() == 1ul);
    BOOST_TEST(
        is_return_of(
            g->getEntryBlock().getTerminator(),
            add_of(
                direct_call_of(exactly(f0)),
                direct_call_of(
                    exactly(f1),
                    call_arg(constant(1), sext),
                    call_arg(const_expr_of(gep_of(array_of(2, is_i8), global_of(cstr("2")), constant(0))))))));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0014_extern_decl/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("rand");
        BOOST_TEST(is_function_of(f, std::tuple{}, is_i32, sext));
    }
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("always_true");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(fnptr_type(std::tuple{}, is_i32), "f")},
                is_i1, zext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    llvm::CmpInst::Predicate::ICMP_EQ,
                    indirect_call_of(exactly(f->getArg(0))),
                    indirect_call_of(exactly(f->getArg(0))))));
    }
    {
        auto const f = pass_result.value()->getFunction("maybe_true");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(fnptr_type(std::tuple{}, is_i32), "f")},
                is_i1, zext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                cmp_of(
                    llvm::CmpInst::Predicate::ICMP_EQ,
                    indirect_call_of(exactly(f->getArg(0))),
                    indirect_call_of(exactly(f->getArg(0))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("always_true")),
                    call_arg(exactly(pass_result.value()->getFunction("zero"))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i1, zext));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("maybe_true")),
                    call_arg(exactly(pass_result.value()->getFunction("rand"))))));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)
// BOOST_AUTO_TEST_CASE(typecheck_error_003)
// BOOST_AUTO_TEST_CASE(typecheck_error_004)

BOOST_AUTO_TEST_SUITE_END()
