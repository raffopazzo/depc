#define BOOST_TEST_MODULE dep0_llvmgen_tests_0001_integral_primitives
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0001_integral_primitives, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0001_integral_primitives/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("f0");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(127)));
    }
    {
        auto const f = pass_result.value()->getFunction("f18");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(32767)));
    }
    {
        auto const f = pass_result.value()->getFunction("f42");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(2147483647)));
    }
    {
        auto const f = pass_result.value()->getFunction("f81");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(9223372036854775807ll)));
    }
    {
        auto const f = pass_result.value()->getFunction("f141");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(255ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("f153");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(65535ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("f169");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(4294967295ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("f195");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(18446744073709551615ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("f235");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(9999999999999999999ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("f237");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(9999999999999999999ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("f238");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(9999999999999999999ul)));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0001_integral_primitives/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("one_i8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("one_i16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("one_i32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("one_i64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("one_u8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("one_u16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("one_u32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("one_u64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_i8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(127)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_i16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(32767)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_i32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(2147483647)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_i64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(9223372036854775807ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_u8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_u16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_u32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_u64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_i8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-128)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_i16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-32768)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_i32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-2147483648ll)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_i64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0x8000'0000'0000'0000ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_u8");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_u16");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i16, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_u32");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_u64");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
}

// BOOST_AUTO_TEST_CASE(parse_error_000)

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)
// BOOST_AUTO_TEST_CASE(typecheck_error_003)
// BOOST_AUTO_TEST_CASE(typecheck_error_004)
// BOOST_AUTO_TEST_CASE(typecheck_error_005)
// BOOST_AUTO_TEST_CASE(typecheck_error_006)
// BOOST_AUTO_TEST_CASE(typecheck_error_007)
// BOOST_AUTO_TEST_CASE(typecheck_error_008)
// BOOST_AUTO_TEST_CASE(typecheck_error_009)
// BOOST_AUTO_TEST_CASE(typecheck_error_010)
// BOOST_AUTO_TEST_CASE(typecheck_error_011)
// BOOST_AUTO_TEST_CASE(typecheck_error_012)
// BOOST_AUTO_TEST_CASE(typecheck_error_013)
// BOOST_AUTO_TEST_CASE(typecheck_error_014)
// BOOST_AUTO_TEST_CASE(typecheck_error_015)
// BOOST_AUTO_TEST_CASE(typecheck_error_016)
// BOOST_AUTO_TEST_CASE(typecheck_error_017)
// BOOST_AUTO_TEST_CASE(typecheck_error_018)
// BOOST_AUTO_TEST_CASE(typecheck_error_019)
// BOOST_AUTO_TEST_CASE(typecheck_error_020)
// BOOST_AUTO_TEST_CASE(typecheck_error_021)
// BOOST_AUTO_TEST_CASE(typecheck_error_022)
// BOOST_AUTO_TEST_CASE(typecheck_error_023)
// BOOST_AUTO_TEST_CASE(typecheck_error_024)
// BOOST_AUTO_TEST_CASE(typecheck_error_025)
// BOOST_AUTO_TEST_CASE(typecheck_error_026)
// BOOST_AUTO_TEST_CASE(typecheck_error_027)
// BOOST_AUTO_TEST_CASE(typecheck_error_028)
// BOOST_AUTO_TEST_CASE(typecheck_error_029)
// BOOST_AUTO_TEST_CASE(typecheck_error_030)
// BOOST_AUTO_TEST_CASE(typecheck_error_031)
// BOOST_AUTO_TEST_CASE(typecheck_error_032)
// BOOST_AUTO_TEST_CASE(typecheck_error_033)
// BOOST_AUTO_TEST_CASE(typecheck_error_034)
// BOOST_AUTO_TEST_CASE(typecheck_error_035)
// BOOST_AUTO_TEST_CASE(typecheck_error_036)
// BOOST_AUTO_TEST_CASE(typecheck_error_037)
// BOOST_AUTO_TEST_CASE(typecheck_error_038)
// BOOST_AUTO_TEST_CASE(typecheck_error_039)
// BOOST_AUTO_TEST_CASE(typecheck_error_040)
// BOOST_AUTO_TEST_CASE(typecheck_error_041)
// BOOST_AUTO_TEST_CASE(typecheck_error_042)
// BOOST_AUTO_TEST_CASE(typecheck_error_043)
// BOOST_AUTO_TEST_CASE(typecheck_error_044)
// BOOST_AUTO_TEST_CASE(typecheck_error_045)
// BOOST_AUTO_TEST_CASE(typecheck_error_046)
// BOOST_AUTO_TEST_CASE(typecheck_error_047)
// BOOST_AUTO_TEST_CASE(typecheck_error_048)
// BOOST_AUTO_TEST_CASE(typecheck_error_049)
// BOOST_AUTO_TEST_CASE(typecheck_error_050)
// BOOST_AUTO_TEST_CASE(typecheck_error_051)
// BOOST_AUTO_TEST_CASE(typecheck_error_052)
// BOOST_AUTO_TEST_CASE(typecheck_error_053)
// BOOST_AUTO_TEST_CASE(typecheck_error_054)
// BOOST_AUTO_TEST_CASE(typecheck_error_055)
// BOOST_AUTO_TEST_CASE(typecheck_error_056)
// BOOST_AUTO_TEST_CASE(typecheck_error_057)
// BOOST_AUTO_TEST_CASE(typecheck_error_058)
// BOOST_AUTO_TEST_CASE(typecheck_error_059)
// BOOST_AUTO_TEST_CASE(typecheck_error_060)
// BOOST_AUTO_TEST_CASE(typecheck_error_061)
// BOOST_AUTO_TEST_CASE(typecheck_error_062)
// BOOST_AUTO_TEST_CASE(typecheck_error_063)
// BOOST_AUTO_TEST_CASE(typecheck_error_064)
// BOOST_AUTO_TEST_CASE(typecheck_error_065)
// BOOST_AUTO_TEST_CASE(typecheck_error_066)
// BOOST_AUTO_TEST_CASE(typecheck_error_067)
// BOOST_AUTO_TEST_CASE(typecheck_error_068)
// BOOST_AUTO_TEST_CASE(typecheck_error_069)
// BOOST_AUTO_TEST_CASE(typecheck_error_070)
// BOOST_AUTO_TEST_CASE(typecheck_error_071)
// BOOST_AUTO_TEST_CASE(typecheck_error_072)
// BOOST_AUTO_TEST_CASE(typecheck_error_073)
// BOOST_AUTO_TEST_CASE(typecheck_error_074)
// BOOST_AUTO_TEST_CASE(typecheck_error_075)
// BOOST_AUTO_TEST_CASE(typecheck_error_076)
// BOOST_AUTO_TEST_CASE(typecheck_error_077)
// BOOST_AUTO_TEST_CASE(typecheck_error_078)
// BOOST_AUTO_TEST_CASE(typecheck_error_079)
// BOOST_AUTO_TEST_CASE(typecheck_error_080)
// BOOST_AUTO_TEST_CASE(typecheck_error_081)
// BOOST_AUTO_TEST_CASE(typecheck_error_082)
// BOOST_AUTO_TEST_CASE(typecheck_error_083)
// BOOST_AUTO_TEST_CASE(typecheck_error_084)
// BOOST_AUTO_TEST_CASE(typecheck_error_085)
// BOOST_AUTO_TEST_CASE(typecheck_error_086)
// BOOST_AUTO_TEST_CASE(typecheck_error_087)
// BOOST_AUTO_TEST_CASE(typecheck_error_088)
// BOOST_AUTO_TEST_CASE(typecheck_error_089)
// BOOST_AUTO_TEST_CASE(typecheck_error_090)
// BOOST_AUTO_TEST_CASE(typecheck_error_091)
// BOOST_AUTO_TEST_CASE(typecheck_error_092)
// BOOST_AUTO_TEST_CASE(typecheck_error_093)
// BOOST_AUTO_TEST_CASE(typecheck_error_094)
// BOOST_AUTO_TEST_CASE(typecheck_error_095)
// BOOST_AUTO_TEST_CASE(typecheck_error_096)
// BOOST_AUTO_TEST_CASE(typecheck_error_097)
// BOOST_AUTO_TEST_CASE(typecheck_error_098)
// BOOST_AUTO_TEST_CASE(typecheck_error_099)
// BOOST_AUTO_TEST_CASE(typecheck_error_100)
// BOOST_AUTO_TEST_CASE(typecheck_error_101)
// BOOST_AUTO_TEST_CASE(typecheck_error_102)
// BOOST_AUTO_TEST_CASE(typecheck_error_103)
// BOOST_AUTO_TEST_CASE(typecheck_error_104)
// BOOST_AUTO_TEST_CASE(typecheck_error_105)
// BOOST_AUTO_TEST_CASE(typecheck_error_106)
// BOOST_AUTO_TEST_CASE(typecheck_error_107)
// BOOST_AUTO_TEST_CASE(typecheck_error_108)
// BOOST_AUTO_TEST_CASE(typecheck_error_109)
// BOOST_AUTO_TEST_CASE(typecheck_error_110)
// BOOST_AUTO_TEST_CASE(typecheck_error_111)
// BOOST_AUTO_TEST_CASE(typecheck_error_112)
// BOOST_AUTO_TEST_CASE(typecheck_error_113)
// BOOST_AUTO_TEST_CASE(typecheck_error_114)
// BOOST_AUTO_TEST_CASE(typecheck_error_115)
// BOOST_AUTO_TEST_CASE(typecheck_error_116)
// BOOST_AUTO_TEST_CASE(typecheck_error_117)
// BOOST_AUTO_TEST_CASE(typecheck_error_118)
// BOOST_AUTO_TEST_CASE(typecheck_error_119)
// BOOST_AUTO_TEST_CASE(typecheck_error_120)
// BOOST_AUTO_TEST_CASE(typecheck_error_121)
// BOOST_AUTO_TEST_CASE(typecheck_error_122)
// BOOST_AUTO_TEST_CASE(typecheck_error_123)
// BOOST_AUTO_TEST_CASE(typecheck_error_124)
// BOOST_AUTO_TEST_CASE(typecheck_error_125)

BOOST_AUTO_TEST_SUITE_END()
