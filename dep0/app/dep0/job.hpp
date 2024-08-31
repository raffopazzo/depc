#pragma once

#include <llvm/Support/CodeGen.h>
#include <llvm/Target/TargetMachine.h>

#include <filesystem>
#include <functional>
#include <variant>
#include <vector>

struct job_t
{
    struct typecheck_t
    {
        std::vector<std::filesystem::path> input_files;
        bool no_prelude;
    };
    struct print_ast_t
    {
        std::vector<std::filesystem::path> input_files;
        bool no_prelude;
        bool skip_transformations;
    };
    struct emit_llvm_t
    {
        std::vector<std::filesystem::path> input_files;
        bool no_prelude;
        bool skip_transformations;
        bool unverified;
    };
    struct compile_only_t
    {
        std::vector<std::filesystem::path> input_files;
        bool no_prelude;
        bool skip_transformations;
        std::reference_wrapper<llvm::TargetMachine> machine;
        llvm::CodeGenFileType file_type;
    };
    struct compile_and_link_t
    {
        std::vector<std::filesystem::path> input_files;
        std::filesystem::path out_file_name;
        bool no_prelude;
        bool skip_transformations;
        std::reference_wrapper<llvm::TargetMachine> machine;
    };
    using value_t = std::variant<typecheck_t, print_ast_t, emit_llvm_t, compile_only_t, compile_and_link_t>;
    value_t value;
};

int run(job_t const&);
