/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/parser/ast.hpp"
#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/temp_file.hpp"
#include "dep0/unique_ref.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

#include <filesystem>

struct parser_stage_t;
struct typecheck_stage_t;
struct transform_stage_t;
struct llvmgen_stage_t;
struct compile_stage_t;

template <typename... Stages>
struct pipeline_t;

using parser_pipeline_t     = pipeline_t<parser_stage_t>;
using typecheck_pipeline_t  = pipeline_t<parser_stage_t, typecheck_stage_t>;
using transform_pipeline_t  = pipeline_t<parser_stage_t, typecheck_stage_t, transform_stage_t>;
using llvmgen_pipeline_t    = pipeline_t<parser_stage_t, typecheck_stage_t, transform_stage_t, llvmgen_stage_t>;
using compile_pipeline_t    = pipeline_t<parser_stage_t, typecheck_stage_t, transform_stage_t, llvmgen_stage_t, compile_stage_t>;

template <typename... Stages>
pipeline_t<Stages...> build_pipeline(Stages&&...);

// parser
struct parser_stage_t
{
};

template <>
struct pipeline_t<parser_stage_t>
{
    explicit pipeline_t(parser_stage_t);

    dep0::expected<dep0::parser::module_t> run(std::filesystem::path const&) const;
};


// typecheck

struct typecheck_stage_t
{
    bool no_prelude = false;
};

template <>
struct pipeline_t<parser_stage_t, typecheck_stage_t> : parser_pipeline_t
{
    typecheck_stage_t options;

    pipeline_t(parser_stage_t, typecheck_stage_t);

    dep0::expected<dep0::typecheck::module_t> run(std::filesystem::path const&) const;
};

// transform
struct transform_stage_t
{
    bool skip = false;
};

template <>
struct pipeline_t<parser_stage_t, typecheck_stage_t, transform_stage_t>
    : typecheck_pipeline_t
{
    transform_stage_t options;

    pipeline_t(parser_stage_t, typecheck_stage_t, transform_stage_t);

    dep0::expected<dep0::typecheck::module_t> run(std::filesystem::path const&) const;
};

// llvmgen

struct llvmgen_stage_t
{
    std::reference_wrapper<llvm::TargetMachine> machine;
    std::reference_wrapper<llvm::LLVMContext> llvm_context;
    bool unverified = false;
};

template <>
struct pipeline_t<parser_stage_t, typecheck_stage_t, transform_stage_t, llvmgen_stage_t>
    : transform_pipeline_t
{
    llvmgen_stage_t options;

    pipeline_t(parser_stage_t, typecheck_stage_t, transform_stage_t, llvmgen_stage_t);

    dep0::expected<dep0::unique_ref<llvm::Module>> run(std::filesystem::path const&) const;
};

// compile

struct compile_stage_t
{
    std::reference_wrapper<llvm::TargetMachine> machine;
    llvm::CodeGenFileType out_file_type = llvm::CodeGenFileType::CGFT_ObjectFile;
};

template <>
struct pipeline_t<parser_stage_t, typecheck_stage_t, transform_stage_t, llvmgen_stage_t, compile_stage_t> :
    llvmgen_pipeline_t
{
    compile_stage_t options;

    pipeline_t(parser_stage_t, typecheck_stage_t, transform_stage_t, llvmgen_stage_t, compile_stage_t);

    dep0::expected<dep0::temp_file_t> run(std::filesystem::path const&) const;
};

template <typename... Stages>
pipeline_t<Stages...> build_pipeline(Stages&&... stages)
{
    return pipeline_t<Stages...>(std::forward<Stages>(stages)...);
}
