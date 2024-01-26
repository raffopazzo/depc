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

    static constexpr std::string_view llvm_memcpy_name = "llvm.memcpy.p0i8.p0i8.i64";

    boost::test_tools::predicate_result pass(std::filesystem::path);
};
