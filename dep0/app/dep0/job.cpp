#include "job.hpp"

#include "failure.hpp"
#include "pipeline.hpp"

#include "dep0/match.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/link/link.hpp"

#include <llvm/ADT/Triple.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/ToolOutputFile.h>

#include <iostream>

static std::filesystem::path append_extension(std::filesystem::path f, std::string_view const extension)
{
    return extension.starts_with('.') ? f.concat(extension) : f.concat(".").concat(extension);
}

int run(job_t const& job)
{
    return dep0::match(
        job.value,
        [] (job_t::typecheck_t const& job)
        {
            auto const pipeline =
                build_pipeline(
                    parser_stage_t{},
                    typecheck_stage_t{
                        .no_prelude = job.no_prelude
                    });
            for (auto const& f: job.input_files)
                if (auto const result = pipeline.run(f))
                    llvm::WithColor::note(llvm::outs(), f.native()) << "typechecks correctly" << '\n';
                else
                    return failure(f, "typecheck error", result.error());
            return 0;
        },
        [] (job_t::print_ast_t const& job)
        {
            auto const pipeline =
                build_pipeline(
                    parser_stage_t{},
                    typecheck_stage_t{
                        .no_prelude = job.no_prelude
                    },
                    transform_stage_t{
                        .skip = job.skip_transformations
                    });
            for (auto const& f: job.input_files)
                if (auto const result = pipeline.run(f))
                    dep0::ast::pretty_print(std::cout, *result) << std::endl;
                else
                    return failure(f, "typecheck error", result.error());
            return 0;
        },
        [] (job_t::emit_llvm_t const& job)
        {
            llvm::LLVMContext llvm_context;
            auto const pipeline =
                build_pipeline(
                    parser_stage_t{},
                    typecheck_stage_t{
                        .no_prelude = job.no_prelude
                    },
                    transform_stage_t{
                        .skip = job.skip_transformations
                    },
                    llvmgen_stage_t{
                        .llvm_context = std::ref(llvm_context),
                        .unverified = job.unverified
                    });
            for (auto const& f: job.input_files)
                if (auto result = pipeline.run(f))
                {
                    std::error_code ec;
                    auto const output_file = append_extension(f, ".ll");
                    auto out = llvm::ToolOutputFile(output_file.native(), ec, llvm::sys::fs::OF_Text);
                    if (ec)
                        return failure(f, "error opening output file");
                    llvm::legacy::PassManager pass_manager;
                    pass_manager.add(llvm::createPrintModulePass(out.os()));
                    pass_manager.run(result->get());
                    out.keep();
                }
                else
                    return failure(f, "llvmgen error", result.error());
            return 0;
        },
        [] (job_t::compile_only_t const& job)
        {
            llvm::LLVMContext llvm_context;
            auto const pipeline =
                build_pipeline(
                    parser_stage_t{},
                    typecheck_stage_t{
                        .no_prelude = job.no_prelude
                    },
                    transform_stage_t{
                        .skip = job.skip_transformations
                    },
                    llvmgen_stage_t{
                        .llvm_context = std::ref(llvm_context),
                        .unverified = false
                    },
                    compile_stage_t{
                        .machine = job.machine,
                        .out_file_type = job.file_type
                    });
            for (auto const& f: job.input_files)
                if (auto obj = pipeline.run(f))
                {
                    auto const dest = append_extension(f, job.file_type == llvm::CGFT_AssemblyFile ? ".s" : ".o");
                    if (auto const rename = obj->rename_and_keep(dest); not rename)
                        return failure(f, "compile error", rename.error());
                }
                else
                    return failure(f, "compile error", obj.error());
            return 0;
        },
        [] (job_t::compile_and_link_t const& job)
        {
            llvm::LLVMContext llvm_context;
            auto const pipeline =
                build_pipeline(
                    parser_stage_t{},
                    typecheck_stage_t{
                        .no_prelude = job.no_prelude
                    },
                    transform_stage_t{
                        .skip = job.skip_transformations
                    },
                    llvmgen_stage_t{
                        .llvm_context = std::ref(llvm_context),
                        .unverified = false
                    },
                    compile_stage_t{
                        .machine = job.machine
                    });
            std::vector<dep0::temp_file_t> obj_files; // automatically delete object files after linking
            std::vector<std::filesystem::path> obj_file_paths;
            for (auto const& f: job.input_files)
                if (auto obj = pipeline.run(f))
                {
                    obj_file_paths.push_back(obj->path());
                    obj_files.push_back(std::move(*obj));
                }
                else
                    return failure(f, "compile error", obj.error());
            auto const host_triple = llvm::Triple(llvm::sys::getProcessTriple());
            auto result =
                dep0::link::link(
                    obj_file_paths,
                    job.machine.get().getTargetTriple(),
                    host_triple);
            if (not result)
                return failure(job.out_file_name, "link error", result.error());
            if (auto const rename = result->rename_and_keep(job.out_file_name); not rename)
                return failure(job.out_file_name, "link error", rename.error());
            return 0;
        });
}
