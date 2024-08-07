#include "llvmgen_tests_fixture.hpp"

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/environment.hpp"
#include "dep0/transform/beta_delta_normalization.hpp"

#include "dep0/llvmgen/gen.hpp"

/**
 * Helper function used to ensure that the base environment is constructed only once for an entire test suite.
 * This is helpful because constructing a fresh base environment for each test would make running the test suite
 * unnecessarily slow, because we would need to parse and check the prelude for each test.
 * Moreover, instead of using a global variable, a function with a static local is preferred because apparently
 * ANTLR4 seems to also initialise some global which might not have been initialised yet when parsing the prelude.
 */
static dep0::typecheck::env_t const& get_base_env()
{
    static auto const env = dep0::typecheck::make_base_env().value();
    return env;
}

std::vector<llvm::BasicBlock const*> LLVMGenTestsFixture::get_blocks(llvm::Function const& f)
{
    std::vector<llvm::BasicBlock const*> blocks;
    blocks.reserve(f.size());
    for (llvm::BasicBlock const& b: f)
        blocks.push_back(&b);
    return blocks;
}

std::vector<llvm::Instruction const*> LLVMGenTestsFixture::get_instructions(llvm::BasicBlock const& b)
{
    std::vector<llvm::Instruction const*> instructions;
    instructions.reserve(b.size());
    for (llvm::Instruction const& x: b)
        instructions.push_back(&x);
    return instructions;
}

boost::test_tools::predicate_result LLVMGenTestsFixture::pass(std::filesystem::path const file)
{
    auto parse_result = dep0::parser::parse(testfiles / file);
    if (parse_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), parse_result.error());
        return res;
    }
    auto check_result = dep0::typecheck::check(get_base_env(), *parse_result);
    if (check_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), check_result.error());
        return res;
    }
    if (apply_beta_delta_normalization)
    {
        dep0::transform::beta_delta_normalization_t beta_delta_normalization{};
        auto transform_result = beta_delta_normalization(*check_result);
        if (transform_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), transform_result.error());
            return res;
        }
    }
    auto gen_result = dep0::llvmgen::gen(llvm_ctx, "test.depc", *check_result);
    if (gen_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), gen_result.error());
        return res;
    }
    pass_result.emplace(std::move(*gen_result));
    return true;
}
