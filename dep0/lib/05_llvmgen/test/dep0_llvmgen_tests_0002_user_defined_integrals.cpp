#define BOOST_TEST_MODULE dep0_llvmgen_tests_0002_user_defined_integrals
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0002_user_defined_integrals, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("h");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(23)));
    }
    {
        auto const f = pass_result.value()->getFunction("n");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(f->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(123456789ul)));
    }
    {
        auto const f = pass_result.value()->getFunction("d");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i64, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1234567890123456789ll)));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("negative");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
    {
        auto const f = pass_result.value()->getFunction("zero");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("max_sign");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(1)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_sign");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        BOOST_TEST(is_direct_call(inst[0], exactly(pass_result.value()->getFunction("max_sign"))));
        BOOST_TEST(is_add_of(inst[1], exactly(inst[0]), constant(1)));
        BOOST_TEST(is_cmp(inst[2], llvm::CmpInst::ICMP_SGT, exactly(inst[1]), constant(1)));
        BOOST_TEST(is_select_of(inst[3], exactly(inst[2]), constant(-1), exactly(inst[1])));
        BOOST_TEST(is_return_of(inst[4], exactly(inst[3])));
    }
    {
        auto const f = pass_result.value()->getFunction("max_hour");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(23)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_hour");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 5ul);
        BOOST_TEST(is_direct_call(inst[0], exactly(pass_result.value()->getFunction("max_hour"))));
        BOOST_TEST(is_add_of(inst[1], exactly(inst[0]), constant(1)));
        BOOST_TEST(is_cmp(inst[2], llvm::CmpInst::ICMP_UGT, exactly(inst[1]), constant(23)));
        BOOST_TEST(is_select_of(inst[3], exactly(inst[2]), constant(0), exactly(inst[1])));
        BOOST_TEST(is_return_of(inst[4], exactly(inst[3])));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("min_hour");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(0)));
    }
    {
        auto const f = pass_result.value()->getFunction("min_sign");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i8, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(-1)));
    }
}

// BOOST_AUTO_TEST_CASE(parse_error_000)
// BOOST_AUTO_TEST_CASE(parse_error_001)
// BOOST_AUTO_TEST_CASE(parse_error_002)
// BOOST_AUTO_TEST_CASE(parse_error_003)
// BOOST_AUTO_TEST_CASE(parse_error_004)
// BOOST_AUTO_TEST_CASE(parse_error_005)
// BOOST_AUTO_TEST_CASE(parse_error_006)
// BOOST_AUTO_TEST_CASE(parse_error_007)
// BOOST_AUTO_TEST_CASE(parse_error_008)
// BOOST_AUTO_TEST_CASE(parse_error_009)
// BOOST_AUTO_TEST_CASE(parse_error_010)

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)
// BOOST_AUTO_TEST_CASE(typecheck_error_002)
// BOOST_AUTO_TEST_CASE(typecheck_error_003)
// BOOST_AUTO_TEST_CASE(typecheck_error_004)
// BOOST_AUTO_TEST_CASE(typecheck_error_005)
// BOOST_AUTO_TEST_CASE(typecheck_error_006)
// BOOST_AUTO_TEST_CASE(typecheck_error_007)
// BOOST_AUTO_TEST_CASE(typecheck_error_008)

BOOST_AUTO_TEST_SUITE_END()
