#define BOOST_TEST_MODULE dep0_llvmgen_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/transform/reify_return_unit.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/llvmgen/gen.hpp"

#include <filesystem>
#include <cstdlib>

namespace dep0
{

std::ostream& operator<<(std::ostream& os, expected<parser::module_t> const& x)
{
    if (x) return os << "Parsing ok";
    else return pretty_print(os << std::endl, x.error());
}

std::ostream& operator<<(std::ostream& os, expected<unique_ref<llvm::Module>> const& x)
{
    if (x) return os << "Codegen ok";
    else return pretty_print(os << std::endl, x.error());
}

}

std::string toString(llvm::Module const& m)
{
    std::string str;
    auto os = llvm::raw_string_ostream(str);
    m.print(os, nullptr);
    return str;
}

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    llvm::LLVMContext llvm_ctx;

    dep0::typecheck::module_t open(std::filesystem::path const& file)
    {
        auto const module = dep0::parser::parse(testfiles / file);
        BOOST_TEST_REQUIRE(module);
        dep0::typecheck::tt::context_t ctx;
        auto const checked = dep0::typecheck::check(ctx, *module);
        BOOST_TEST_REQUIRE(checked.has_value());
        return *checked;
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000)
{
    auto const module = open("test_0000.depc");
    auto const result = dep0::llvmgen::gen(llvm_ctx, "test.depc", module);
    BOOST_TEST_REQUIRE(result.has_value());
    BOOST_TEST(result->get().size() == 0ul);
}

BOOST_AUTO_TEST_CASE(test_0001)
{
    auto const module = open("test_0001.depc");
    auto const result = dep0::llvmgen::gen(llvm_ctx, "test.depc", module);
    BOOST_TEST_REQUIRE(result.has_value());
    BOOST_TEST(result->get().size() == 1ul);
    auto const& f = *result->get().begin();
    auto const* p = result->get().getFunction("main");
    BOOST_TEST(&f == p);
    // TODO should perhaps test properties of f
}

// BOOST_AUTO_TEST_CASE(test_0002) doesn't type check

BOOST_AUTO_TEST_CASE(test_0003)
{
    auto module = open("test_0003.depc");
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module).has_error());
    BOOST_TEST(dep0::transform::reify_return_unit(module).has_value());
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module));
}

// BOOST_AUTO_TEST_CASE(test_0004) doesn't type check

BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", open("test_0005.depc"))); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", open("test_0006.depc"))); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", open("test_0007.depc"))); }
// BOOST_AUTO_TEST_CASE(test_0008) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0009) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0010) doesn't type check
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", open("test_0011.depc"))); }
// BOOST_AUTO_TEST_CASE(test_0012) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0013) doesn't type check
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", open("test_0014.depc"))); }
BOOST_AUTO_TEST_CASE(test_0015)
{
    auto module = open("test_0015.depc");
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module).has_error());
    BOOST_TEST(dep0::transform::reify_return_unit(module).has_value());
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module));
}
// BOOST_AUTO_TEST_CASE(test_0016) doesn't type check
// BOOST_AUTO_TEST_CASE(test_0017) doesn't type check

BOOST_AUTO_TEST_CASE(test_0018)
{
    auto module = open("test_0018.depc");
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module).has_error());
    BOOST_TEST(dep0::transform::reify_return_unit(module).has_value());
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module));
}
BOOST_AUTO_TEST_CASE(test_0019)
{
    auto module = open("test_0019.depc");
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module).has_error());
    BOOST_TEST(dep0::transform::reify_return_unit(module).has_value());
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module));
}

BOOST_AUTO_TEST_CASE(test_0020)
{
    auto module = open("test_0020.depc");
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module).has_error());
    BOOST_TEST(dep0::transform::reify_return_unit(module).has_value());
    BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", module));
}

BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(dep0::llvmgen::gen(llvm_ctx, "test.depc", open("test_0021.depc"))); }

BOOST_AUTO_TEST_SUITE_END()
