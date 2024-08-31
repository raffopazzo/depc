#include "failure.hpp"
#include "job.hpp"
#include "pipeline.hpp"

#include <llvm/CodeGen/CommandFlags.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Target/TargetMachine.h>

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

static llvm::codegen::RegisterCodeGenFlags WTF;

static dep0::expected<std::reference_wrapper<llvm::TargetMachine>> setup_target_machine(llvm::Triple& target_triple)
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

    // options for all sub-commands
    auto const input_files = cl::list<std::string>(cl::Positional, cl::desc("<input-files>"));
    auto const assemble_only =
        cl::opt<bool>(
            "S",
            cl::init(false),
            cl::desc("Stop after generating assembly code; do not write object code"));
    auto const compile_only =
        cl::opt<bool>(
            "c",
            cl::init(false),
            cl::desc("Stop after compiling the input files into object code; do not link the final executable"));
    auto const typecheck_only =
        cl::opt<bool>(
            "t",
            cl::init(false),
            cl::desc("Stop after typechecking the input files"));
    auto const print_ast =
        cl::opt<bool>(
            "print-ast",
            cl::init(false),
            cl::desc(
                "Print the typechecked AST to screen after applying all transformations"
                " (unless --skip-transformations is set)"));
    auto const emit_llvm =
        cl::opt<bool>(
            "emit-llvm",
            cl::init(false),
            cl::desc("Emit (verified) LLVM IR code"));
    auto const emit_llvm_unverified =
        cl::opt<bool>(
            "emit-llvm-unverified",
            cl::init(false),
            cl::desc("Emit unverified LLVM IR code; only useful when debugging the llvmgen module"));
    auto const no_prelude =
        cl::opt<bool>(
            "no-prelude",
            cl::init(false),
            cl::desc("Do not pre-import the prelude module"));
    auto const skip_transformations =
        cl::opt<bool>(
            "skip-transformations",
            cl::init(false),
            cl::desc("Skip the transformations pipeline stage"));

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
        return print_ast
            ? run(job_t{job_t::print_ast_t{
                .input_files = input_file_paths,
                .no_prelude = no_prelude,
                .skip_transformations = skip_transformations
                }})
            : run(job_t{job_t::typecheck_t{
                .input_files = input_file_paths,
                .no_prelude = no_prelude
                }});
    if (print_ast)
        llvm::WithColor::warning() << "--print-ast can only be used with -t; will be ignored\n";

    auto const file_type = llvm::codegen::getExplicitFileType().getValueOr(llvm::CodeGenFileType::CGFT_ObjectFile);
    auto target_triple =
        llvm::Triple(
            mtriple.empty()
            ? llvm::sys::getDefaultTargetTriple()
            : llvm::Triple::normalize(mtriple));
    auto const machine = setup_target_machine(target_triple);

    if (not machine)
        return failure("failed to create target machine");
    if (emit_llvm or emit_llvm_unverified)
        return run(job_t{job_t::emit_llvm_t{
            .input_files = input_file_paths,
            .no_prelude =  no_prelude,
            .skip_transformations = skip_transformations,
            .unverified = emit_llvm_unverified
        }});
    if (file_type == llvm::CodeGenFileType::CGFT_AssemblyFile or assemble_only)
        return run(job_t{job_t::compile_only_t{
            .input_files = input_file_paths,
            .no_prelude = no_prelude,
            .skip_transformations = skip_transformations,
            .machine = std::ref(*machine),
            .file_type = llvm::CodeGenFileType::CGFT_AssemblyFile
        }});
    if (compile_only)
        return run(job_t{job_t::compile_only_t{
            .input_files = input_file_paths,
            .no_prelude = no_prelude,
            .skip_transformations = skip_transformations,
            .machine = std::ref(*machine),
            .file_type = llvm::CodeGenFileType::CGFT_ObjectFile
        }});
    return run(job_t{job_t::compile_and_link_t{
        .input_files = input_file_paths,
        .out_file_name = "a.out",
        .no_prelude = no_prelude,
        .skip_transformations = skip_transformations,
        .machine = std::ref(*machine)
    }});
}
