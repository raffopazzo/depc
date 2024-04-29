#define BOOST_TEST_MODULE dep0_llvmgen_tests_0003_function_arguments
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0003_function_arguments, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("id");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0ul))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("id");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i32, "x", sext)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0ul))));
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("id")),
                    call_arg(constant(0), sext))));
    }
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST(pass("0003_function_arguments/pass_002.depc"));
    {
        auto const f = pass_result.value()->getFunction("first");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "x", sext), arg_of(is_i32, "y", sext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0ul))));
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(
                    exactly(pass_result.value()->getFunction("first")),
                    call_arg(constant(0), sext),
                    call_arg(constant(1), sext))));
    }
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_003.depc"));
    {
        auto const f = pass_result.value()->getFunction("first");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{arg_of(is_i32, "x", sext), arg_of(is_i32, "y", zext)},
                is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0ul))));
    }
    {
        auto const f = pass_result.value()->getFunction("main");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 2ul);
        BOOST_TEST(
            is_direct_call(
                inst[0ul],
                exactly(pass_result.value()->getFunction("first")),
                call_arg(constant(0), sext),
                call_arg(constant(1), zext)));
        BOOST_TEST(is_return_of(inst[1ul], constant(0)));
    }
}

BOOST_AUTO_TEST_CASE(pass_004)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_004.depc"));
    {
        auto const f = pass_result.value()->getFunction("unit");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), is_zeroinitializer));
    }
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(struct_of())}, struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), is_zeroinitializer));
    }
    {
        auto const f = pass_result.value()->getFunction("g1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 3ul);
        auto const call1 = inst[0ul];
        auto const call2 = inst[1ul];
        auto const ret   = inst[2ul];
        BOOST_TEST(is_direct_call(call1, exactly(pass_result.value()->getFunction("unit"))));
        BOOST_TEST(is_direct_call(call2, exactly(pass_result.value()->getFunction("f1")), call_arg(exactly(call1))));
        BOOST_TEST(is_return_of(ret, is_zeroinitializer));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(struct_of(), "x")}, struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), is_zeroinitializer));
    }
    {
        auto const f = pass_result.value()->getFunction("g2");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, struct_of()));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const inst = get_instructions(f->getEntryBlock());
        BOOST_TEST_REQUIRE(inst.size() == 3ul);
        auto const call1 = inst[0ul];
        auto const call2 = inst[1ul];
        auto const ret   = inst[2ul];
        BOOST_TEST(is_direct_call(call1, exactly(pass_result.value()->getFunction("unit"))));
        BOOST_TEST(is_direct_call(call2, exactly(pass_result.value()->getFunction("f2")), call_arg(exactly(call1))));
        BOOST_TEST(is_return_of(ret, is_zeroinitializer));
    }
}

BOOST_AUTO_TEST_CASE(pass_005)
{
    BOOST_TEST_REQUIRE(pass("0003_function_arguments/pass_005.depc"));
    {
        auto const f = pass_result.value()->getFunction("f");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(is_i1, "f", zext)}, is_i1, zext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), exactly(f->getArg(0ul))));
    }
}

// BOOST_AUTO_TEST_CASE(typecheck_error_000)
// BOOST_AUTO_TEST_CASE(typecheck_error_001)

BOOST_AUTO_TEST_SUITE_END()
