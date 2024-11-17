#define BOOST_TEST_MODULE dep0_llvmgen_tests_0020_builtins
#include <boost/test/included/unit_test.hpp>

#include "llvmgen_tests_fixture.hpp"
#include "llvm_helpers.hpp"
#include "llvm_predicates.hpp"

using namespace dep0::llvmgen::testing;

static auto const nonnull = std::vector{llvm::Attribute::NonNull};
static auto const sext = std::vector{llvm::Attribute::SExt};
static auto const zext = std::vector{llvm::Attribute::ZExt};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests_0020_builtins, LLVMGenTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0020_builtins/pass_000.depc"));
    {
        auto const f = pass_result.value()->getFunction("sum");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = exactly(f->getArg(0ul));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                add_of(
                    load_of(is_i32, gep_of(is_i32, xs, constant(0)), align_of(4)),
                    load_of(is_i32, gep_of(is_i32, xs, constant(1)), align_of(4)))));
    }
    auto const sum = exactly(pass_result.value()->getFunction("sum"));
    {
        auto const f = pass_result.value()->getFunction("f1");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        auto const xs = exactly(f->getArg(0ul));
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(sum, call_arg(gep_of(is_i32, xs, constant(1))))));
    }
    {
        auto const f = pass_result.value()->getFunction("f2");
        BOOST_TEST_REQUIRE(
            is_function_of(
                f,
                std::tuple{
                    ret_arg_of(is_i32),
                    arg_of(pointer_to(is_i32), "xs", nonnull),
                    arg_of(pointer_to(is_i32), "ys", nonnull),
                    arg_of(is_i1, "which", zext),
                },
                is_void));
        auto const ret_arg = f->getArg(0ul);
        auto const xs = f->getArg(1ul);
        auto const ys = f->getArg(2ul);
        auto const which = f->getArg(3ul);
        auto const blks = get_blocks(*f);
        BOOST_TEST_REQUIRE(blks.size() == 3ul);
        auto const entry = blks[0];
        auto const then0 = blks[1];
        auto const else0 = blks[2];
        {
            BOOST_TEST_REQUIRE(entry->size() == 1ul);
            BOOST_TEST(is_branch_of(entry->getTerminator(), exactly(which), exactly(then0), exactly(else0)));
        }
        {
            auto const inst = get_instructions(*then0);
            BOOST_TEST_REQUIRE(inst.size() == 5ul);
            auto const sliced = inst[0];
            auto const dst    = inst[1];
            auto const src    = inst[2];
            auto const memcpy = inst[3];
            auto const ret    = inst[4];
            BOOST_TEST(is_gep_of(sliced, is_i32, exactly(xs), constant(1)));
            BOOST_TEST(is_bitcast_of(dst, exactly(ret_arg), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(sliced), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(
                is_direct_call(
                    memcpy,
                    exactly(pass_result.value()->getFunction(llvm_memcpy_name)),
                    call_arg(exactly(dst), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(src), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(constant(8)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
        {
            auto const inst = get_instructions(*else0);
            BOOST_TEST_REQUIRE(inst.size() == 5ul);
            auto const sliced = inst[0];
            auto const dst    = inst[1];
            auto const src    = inst[2];
            auto const memcpy = inst[3];
            auto const ret    = inst[4];
            BOOST_TEST(is_gep_of(sliced, is_i32, exactly(ys), constant(1)));
            BOOST_TEST(is_bitcast_of(dst, exactly(ret_arg), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(is_bitcast_of(src, exactly(sliced), pointer_to(is_i32), pointer_to(is_i8)));
            BOOST_TEST(
                is_direct_call(
                    memcpy,
                    exactly(pass_result.value()->getFunction(llvm_memcpy_name)),
                    call_arg(exactly(dst), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(exactly(src), {llvm::Attribute::Alignment}, llvm::Align(4)),
                    call_arg(constant(8)),
                    call_arg(constant(false))));
            BOOST_TEST(is_return_of_void(ret));
        }
    }
    {
        auto const f = pass_result.value()->getFunction("f3");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{arg_of(pointer_to(is_i32), "xs", nonnull)}, is_i32, sext));
        auto const xs = exactly(f->getArg(0ul));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(
            is_return_of(
                f->getEntryBlock().getTerminator(),
                direct_call_of(sum, call_arg(gep_of(is_i32, gep_of(is_i32, xs, constant(4)), constant(2))))));
    }
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    apply_beta_delta_normalization = true;
    BOOST_TEST_REQUIRE(pass("0020_builtins/pass_001.depc"));
    {
        auto const f = pass_result.value()->getFunction("seven");
        BOOST_TEST_REQUIRE(is_function_of(f, std::tuple{}, is_i32, sext));
        BOOST_TEST_REQUIRE(f->size() == 1ul);
        BOOST_TEST(is_return_of(f->getEntryBlock().getTerminator(), constant(7)));
    }
}

BOOST_AUTO_TEST_SUITE_END()
