#include "job.hpp"

#include "failure.hpp"
#include "pipeline.hpp"

#include "dep0/match.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/llvmgen/gen.hpp"
#include "dep0/linker/link.hpp"

#include <llvm/ADT/Triple.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/ToolOutputFile.h>

#include <iostream>

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
                        .verify = job.unverified ? dep0::llvmgen::verify_t::no : dep0::llvmgen::verify_t::yes
                    });
            for (auto const& f: job.input_files)
                if (auto result = pipeline.run(f))
                {
                    std::error_code ec;
                    std::filesystem::path const output_file(f.native() + ".ll");
                    auto out = llvm::ToolOutputFile(output_file.native(), ec, llvm::sys::fs::OF_Text);
                    if (ec)
                        return failure(f, "error opening output file");
                    llvm::legacy::PassManager pass_manager;
                    pass_manager.add(llvm::createPrintModulePass(out.os()));
                    pass_manager.run(result->get());
                    out.keep();
                }
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
                        .verify = dep0::llvmgen::verify_t::yes
                    },
                    compile_stage_t{
                        .machine = job.machine,
                        .out_file_type = job.file_type
                    });
            for (auto const& f: job.input_files)
                if (auto const result = pipeline.run(f); not result)
                    return failure(f, "compile error", result.error());
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
                        .verify = dep0::llvmgen::verify_t::yes
                    },
                    compile_stage_t{
                        .machine = job.machine
                    });
            std::vector<std::filesystem::path> obj_files;
            for (auto const& f: job.input_files)
                if (auto const result = pipeline.run(f))
                    obj_files.push_back(*result);
                else
                    return failure(f, "compile error", result.error());
            auto const host_triple = llvm::Triple(llvm::sys::getProcessTriple());
            auto const ok =
                dep0::linker::link(
                    obj_files,
                    job.machine.get().getTargetTriple(),
                    host_triple,
                    job.out_file_name);
            return ok ? 0 : failure(job.out_file_name, "linker error", ok.error());
        });
}
