/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "llvmgen_tests_fixture.hpp"

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/environment.hpp"
#include "dep0/transform/beta_delta_normalization.hpp"

#include "dep0/llvmgen/gen.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>

/**
 * Helper function used to ensure that the base environment is constructed only once for an entire test suite.
 * This is helpful because constructing a fresh base environment for each test would make running the test suite
 * unnecessarily slow, because we would need to parse and check the prelude for each test.
 * Moreover, instead of using a global variable, a function with a static local is preferred because apparently
 * ANTLR4 seems to also initialise some global which might not have been initialised yet when parsing the prelude.
 */
static dep0::typecheck::env_t const& get_base_env()
{
    static auto const env = std::move(dep0::typecheck::make_base_env().value());
    return env;
}

LLVMGenTestsFixture::LLVMGenTestsFixture()
{
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();
}

boost::test_tools::predicate_result LLVMGenTestsFixture::pass(std::filesystem::path const file)
{
    auto const target_triple = std::string("x86_64-unknown-linux-gnu");
    std::string llvm_err;
    auto const target = llvm::TargetRegistry::lookupTarget(target_triple, llvm_err);
    if (not target)
        return dep0::testing::failure("failed to lookup LLVM target from triple `", target_triple, "`: ", llvm_err);
    auto const machine = target->createTargetMachine(target_triple, "", "", {}, {});
    if (not machine)
        return dep0::testing::failure("failed to create target machine");
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
    auto gen_result = dep0::llvmgen::gen(llvm_ctx, "test.depc", *check_result, *machine);
    if (gen_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), gen_result.error());
        return res;
    }
    pass_result.emplace(std::move(*gen_result));
    return true;
}

llvm::Function const* LLVMGenTestsFixture::get_function(std::string_view const name) const
{
    return pass_result ? pass_result.value()->getFunction(name) : nullptr;
}

llvm::StructType const* LLVMGenTestsFixture::get_struct(std::string_view const name) const
{
    if (pass_result)
        for (auto const s : pass_result.value()->getIdentifiedStructTypes())
            if (static_cast<std::string_view>(s->getName()) == name)
                return s;
    return nullptr;
}
