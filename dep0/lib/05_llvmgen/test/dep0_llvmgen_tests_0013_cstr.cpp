#define BOOST_TEST_MODULE dep0_llvmgen_tests_0013_cstr
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0013_cstr, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0013_cstr/pass_000.depc"));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_"), is_zeroinitializer)); // empty string
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.1"), cstr("0")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.2"), cstr("hello world")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.3"), cstr("hello \"world\"")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.4"), cstr("hello 'world'")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.5"), cstr("hello\\world")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.6"), cstr("hello\nworld")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.7"), cstr("hello\tworld")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.8"), cstr("hello\vworld")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.9"), cstr("hello\bworld")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.10"), cstr("hello\fworld")));
    BOOST_TEST(is_global_of(pass_result.value()->getNamedGlobal("$_str_.11"), cstr("hello\rworld")));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = false;
    BOOST_TEST_REQUIRE(pass("0013_cstr/pass_001.depc"));
    auto const f = pass_result.value()->getFunction("f");
    BOOST_TEST(is_function_of(f, std::tuple{arg_of(pointer_to(is_i8), std::nullopt, nonnull)}, struct_of()));
}

BOOST_AUTO_TEST_SUITE_END()
