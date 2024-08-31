#include "pipeline.hpp"

#include "dep0/match.hpp"
#include "dep0/mmap.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/environment.hpp"
#include "dep0/transform/run.hpp"
#include "dep0/transform/beta_delta_normalization.hpp"
#include "dep0/llvmgen/gen.hpp"
#include "dep0/linker/link.hpp"

#include <llvm/CodeGen/CommandFlags.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Target/TargetMachine.h>

#include <filesystem>
#include <iostream>
#include <variant>
#include <vector>

static llvm::codegen::RegisterCodeGenFlags WTF;

static int failure(std::string_view const msg);
static int failure(std::filesystem::path const&, std::string_view const msg);
static int failure(std::filesystem::path const&, std::string_view const msg, dep0::error_t const&);
static int failure(std::filesystem::path const&, std::string_view const msg, dep0::typecheck::error_t const&);

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
    struct emit_ir_t
    {
        std::vector<std::filesystem::path> input_files;
        bool no_prelude;
        bool skip_transformations;
        llvm::Triple target_triple;
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
    using value_t = std::variant<typecheck_t, print_ast_t, emit_ir_t, compile_only_t, compile_and_link_t>;
    value_t value;
};

dep0::expected<std::reference_wrapper<llvm::TargetMachine>> setup_target_machine(llvm::Triple& target_triple)
{
    std::string err;
    if (auto const target = llvm::TargetRegistry::lookupTarget(llvm::codegen::getMArch(), target_triple, err))
        return std::ref(*target->createTargetMachine(
            target_triple.getTriple(),
            llvm::codegen::getCPUStr(),
            llvm::codegen::getFeaturesStr(),
            llvm::codegen::InitTargetOptionsFromCodeGenFlags(target_triple),
            llvm::Optional<llvm::Reloc::Model>(llvm::codegen::getRelocModel())));
    else
        return dep0::error_t(err);
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
        [] (job_t::emit_ir_t const& job)
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

int main(int argc, char** argv)
{
    namespace cl = llvm::cl;

    llvm::InitLLVM LLVM(argc, argv);
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    // options which we want to hide alongside all LLVM options (see below)
    auto const mtriple = cl::opt<std::string>("mtriple", cl::desc("Override target triple for module"));

    // LLVM is nasty enough to register all codegen and target options as visible in the default option category.
    // This clutters up the output of a simple `--help` making it difficult to see DepC options.
    // So let's start afresh by hiding all LLVM options; they can still be retrieved via `--help-hidden`.
    for (auto const& opt: cl::getRegisteredOptions())
        opt.getValue()->setHiddenFlag(cl::OptionHidden::Hidden);

    auto const typecheck_only = cl::SubCommand("check", "Typecheck the input file(s)");
    auto const print_ast = cl::SubCommand(
        "print-ast",
        "Print the typechecked AST to screen after applying all transformations (unless --skip-transformations is set)"
    );

    auto emit_ir = cl::SubCommand("emit-ir", "Emit IR code instead");
    auto const unverified =
        cl::opt<bool>(
            "unverified",
            cl::desc("Emit unverified LLVM IR; only useful when debugging the llvmgen module"),
            cl::init(false),
            cl::sub(emit_ir));

    // options for all sub-commands
    auto depc_options = cl::OptionCategory("DepC Specific Options");
    auto const input_files =
        cl::list<std::string>(
            cl::Positional,
            cl::desc("<input-files>"),
            cl::sub(*cl::AllSubCommands));
    auto const no_prelude =
        cl::opt<bool>(
            "no-prelude",
            cl::desc("Do not pre-import the prelude module"),
            cl::init(false),
            cl::cat(depc_options),
            cl::sub(*cl::AllSubCommands));
    auto const skip_transformations =
        cl::opt<bool>(
            "skip-transformations",
            cl::desc("Skip the transformations pipeline stage"),
            cl::init(false),
            cl::cat(depc_options),
            cl::sub(*cl::AllSubCommands));

    cl::ParseCommandLineOptions(
        argc, argv,
        "dep0 - DepC Bootstrapping Compiler",
        /*Errs*/ nullptr,
        /*EnvVar*/ nullptr,
        /*LongOptionsUseDoubleDash*/ true);

    if (input_files.empty())
        return failure("no input files");

    std::vector<std::filesystem::path> input_file_paths;
    for (auto const& f: input_files)
        input_file_paths.push_back(f);

    if (typecheck_only)
        return run(job_t{job_t::typecheck_t{
            .input_files = input_file_paths,
            .no_prelude = no_prelude
        }});

    if (print_ast)
        return run(job_t{job_t::print_ast_t{
            .input_files = input_file_paths,
            .no_prelude = no_prelude,
            .skip_transformations = skip_transformations
        }});

    auto const file_type = llvm::codegen::getExplicitFileType().getValueOr(llvm::CodeGenFileType::CGFT_ObjectFile);
    auto target_triple =
        llvm::Triple(
            mtriple.empty()
            ? llvm::sys::getDefaultTargetTriple()
            : llvm::Triple::normalize(mtriple));
    auto const machine = setup_target_machine(target_triple);

    if (not machine)
        return failure("failed to create target machine");
    if (emit_ir)
        return run(job_t{job_t::emit_ir_t{
            .input_files = input_file_paths,
            .no_prelude =  no_prelude,
            .skip_transformations = skip_transformations,
            .target_triple = target_triple,
            .unverified = unverified
        }});
    if (file_type == llvm::CodeGenFileType::CGFT_AssemblyFile)
        return run(job_t{job_t::compile_only_t{
            .input_files = input_file_paths,
            .no_prelude = no_prelude,
            .skip_transformations = skip_transformations,
            .machine = std::ref(*machine),
            .file_type = file_type
        }});
    return run(job_t{job_t::compile_and_link_t{
        .input_files = input_file_paths,
        .out_file_name = "a.out",
        .no_prelude = no_prelude,
        .skip_transformations = skip_transformations,
        .machine = std::ref(*machine)
    }});
}

int failure(std::string_view const msg)
{
    llvm::WithColor::error(llvm::errs()) << msg << '\n';
    return 1;
}
int failure(std::filesystem::path const& file_name, std::string_view const msg)
{
    llvm::WithColor::error(llvm::errs(), file_name.native()) << msg << '\n';
    return 1;
}

int failure(std::filesystem::path const& file_name, std::string_view const msg, dep0::error_t const& error)
{
    std::ostringstream str;
    dep0::pretty_print(str, error);
    llvm::WithColor::error(llvm::errs(), file_name.native()) << msg << ": " << str.str() << '\n';
    return 1;
}

int failure(std::filesystem::path const& file_name, std::string_view const msg, dep0::typecheck::error_t const& error)
{
    std::ostringstream str;
    dep0::typecheck::pretty_print(str, error);
    llvm::WithColor::error(llvm::errs(), file_name.native()) << msg << ": " << str.str() << '\n';
    return 1;
}
