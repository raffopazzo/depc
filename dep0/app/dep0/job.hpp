/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <llvm/Support/CodeGen.h>
#include <llvm/Target/TargetMachine.h>

#include <filesystem>
#include <functional>
#include <optional>
#include <variant>
#include <vector>

/**
 * Helper type that allows to decouple parsing of the command line arguments from
 * running the job that the user intended via those command line arguments.
 * The role of the main function is to use command line arguments to construct a job and run it,
 * for example to typecheck the input files or compiling them into the final executable.
 */
struct job_t
{
    /**
     * Runs the parse and typecheck pipeline stages on each input file.
     * If `no_prelude` is set, typechecking will be performed without importing the prelude module;
     * this is useful when typechecking a new prelude module.
     */
    struct typecheck_t
    {
        std::vector<std::filesystem::path> input_files;
        bool no_prelude;
    };

    /**
     * Runs the parse, typecheck and transform pipeline stages on each input file.
     * If a file typechecks correctly, the typechecked AST will be printed out to screen.
     * If `no_prelude` is set, typechecking will be performed without importing the prelude module.
     * If `skip_transformations` is set, the transform stage will not be run.
     */
    struct print_ast_t
    {
        std::vector<std::filesystem::path> input_files;
        bool no_prelude;
        bool skip_transformations;
    };

    /**
     * Runs the parse, typecheck and transform pipeline stages on each input file and
     * writes a new file with the LLVM IR code of the result.
     * An optional output file name can be specified but only if there is a single input file.
     * If `no_prelude` is set, typechecking will be performed without importing the prelude module.
     * If `skip_transformations` is set, the transform stage will not be run.
     * If `unverified` is set, the LLVM IR code generated will not be verified;
     * this is useful when debugging the llvmgen module.
     */
    struct emit_llvm_t
    {
        std::vector<std::filesystem::path> input_files;
        std::optional<std::filesystem::path> out_file_name;
        bool no_prelude;
        bool skip_transformations;
        bool unverified;
    };

    /**
     * Runs the parse, typecheck, transform and compile pipeline stages on each input file but
     * does not perform linking into a final executable.
     * It writes either object or assembly files depending on the value of `file_type`.
     * An optional output file name can be specified but only if there is a single input file.
     * If `no_prelude` is set, typechecking will be performed without importing the prelude module.
     * If `skip_transformations` is set, the transform stage will not be run.
     */
    struct compile_only_t
    {
        std::vector<std::filesystem::path> input_files;
        std::optional<std::filesystem::path> out_file_name;
        bool no_prelude;
        bool skip_transformations;
        std::reference_wrapper<llvm::TargetMachine> machine;
        llvm::CodeGenFileType file_type;
    };

    /**
     * Runs the full pipeline, including linking to produce the final executable.
     * If `no_prelude` is set, typechecking will be performed without importing the prelude module.
     * If `skip_transformations` is set, the transform stage will not be run.
     */
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

/** Runs the given job and returns 0 if it succeeds. */
int run(job_t const&);
