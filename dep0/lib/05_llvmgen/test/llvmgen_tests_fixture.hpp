#pragma once

#include "dep0/unique_ref.hpp"

#include "dep0/testing/predicate.hpp"
#include "dep0/testing/failure.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <boost/test/tools/assertion_result.hpp>

#include <cstdlib>
#include <filesystem>
#include <optional>

struct LLVMGenTestsFixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    llvm::LLVMContext llvm_ctx;
    std::optional<dep0::unique_ref<llvm::Module>> pass_result;
    bool apply_beta_delta_normalization = false;

    boost::test_tools::predicate_result pass(std::filesystem::path);

    template <dep0::testing::Predicate<llvm::Function> F>
    boost::test_tools::predicate_result has_function(std::string_view const name, F&& f)
    {
        auto const function = pass_result.value()->getFunction(name);
        if (not function)
            return dep0::testing::failure("function not found: ", name);
        return std::forward<F>(f)(*function);
    }
};
