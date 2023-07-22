#define BOOST_TEST_MODULE dep0_llvmgen_attr_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/llvmgen/gen.hpp"

#include <filesystem>
#include <cstdlib>

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    llvm::LLVMContext llvm_ctx;

    dep0::unique_ref<llvm::Module> gen(std::filesystem::path const file)
    {
        auto parse_result = dep0::parser::parse(testfiles / file);
        BOOST_TEST_REQUIRE(parse_result.has_value());
        auto check_result = dep0::typecheck::check(*parse_result);
        BOOST_TEST_REQUIRE(check_result.has_value());
        auto gen_result = dep0::llvmgen::gen(llvm_ctx, "test.depc", *check_result);
        BOOST_TEST_REQUIRE(gen_result.has_value());
        return std::move(*gen_result);
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_attr_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0024)
{
    auto m = gen("test_0024.depc");
    auto* f0 = m->getFunction("f0");
    auto* f18 = m->getFunction("f18");
    auto* f42 = m->getFunction("f42");
    auto* f81 = m->getFunction("f81");
    auto* f141 = m->getFunction("f141");
    auto* f153 = m->getFunction("f153");
    auto* f169 = m->getFunction("f169");
    auto* f195 = m->getFunction("f195");
    BOOST_TEST(f0);
    BOOST_TEST(f18);
    BOOST_TEST(f42);
    BOOST_TEST(f81);
    BOOST_TEST(f141);
    BOOST_TEST(f153);
    BOOST_TEST(f169);
    BOOST_TEST(f195);
    if (f0) BOOST_TEST(f0->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    if (f18) BOOST_TEST(f18->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    if (f42) BOOST_TEST(f42->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    if (f81) BOOST_TEST(f81->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::SExt));
    if (f141) BOOST_TEST(f141->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    if (f153) BOOST_TEST(f153->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    if (f169) BOOST_TEST(f169->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
    if (f195) BOOST_TEST(f195->hasAttribute(llvm::AttributeList::ReturnIndex, llvm::Attribute::ZExt));
}

BOOST_AUTO_TEST_SUITE_END()
