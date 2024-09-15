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

namespace fs = std::filesystem;

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

    // LLVM is nasty enough to register all codegen and target options as visible in the default option category.
    // This clutters up the output of a simple `--help` making it difficult to see DepC options.
    // So put all LLVM options in its own category and hide them; they can still be retrieved via `--help-hidden`.
    cl::OptionCategory llvmCat("LLVM Options", "Options available by default with LLVM");
    for (auto const& opt: cl::getRegisteredOptions())
    {
        opt.getValue()->setHiddenFlag(cl::OptionHidden::Hidden);
        opt.getValue()->addCategory(llvmCat);
    }

    // main options - sorted by the actual command line argument, i.e. `--a` before `--b`
    cl::OptionCategory mainCat("Main Options", "These options are the most commonly used ones");
    auto const compile_only =
        cl::opt<bool>(
            "S",
            cl::init(false),
            cl::cat(mainCat),
            cl::desc("Compile only; do not assemble or link"));
    auto const compile_and_assemble =
        cl::opt<bool>(
            "c",
            cl::init(false),
            cl::cat(mainCat),
            cl::desc("Compile and assemble; but do not link"));
    auto const emit_llvm =
        cl::opt<bool>(
            "emit-llvm",
            cl::init(false),
            cl::cat(mainCat),
            cl::desc("Compile only and emit verified LLVM IR code; but do not assemble or link"));
    auto const emit_llvm_unverified =
        cl::opt<bool>(
            "emit-llvm-unverified",
            cl::init(false),
            cl::cat(mainCat),
            cl::desc(
                "Compile only and emit unverified LLVM IR code; but do not assemble or link.\n"
                "This is only useful when debugging the llvmgen module"));
    auto const out_file_name =
        cl::opt<std::string>(
            "o",
            cl::cat(mainCat),
            cl::desc("Specify an output file name. If missing a default one will be used"));
    auto const typecheck_only =
        cl::opt<bool>(
            "t",
            cl::init(false),
            cl::cat(mainCat),
            cl::desc("Typecheck only; do not compile, assemble or link"));

    // extra options - also sorted as above
    cl::OptionCategory extraCat("Secondary Options", "These are extra options that can be useful occasionally");
    auto const mtriple =
        cl::opt<std::string>(
            "mtriple",
            cl::cat(extraCat),
            cl::desc("Override the target triple used during compilation and assembly stages"));
    auto const no_prelude =
        cl::opt<bool>(
            "no-prelude",
            cl::init(false),
            cl::cat(extraCat),
            cl::desc("Do not pre-import the prelude module"));
    auto const print_ast =
        cl::opt<bool>(
            "print-ast",
            cl::init(false),
            cl::cat(extraCat),
            cl::desc(
                "When using -t, print the resulting AST.\n"
                "The AST is printed after the transformation stage, unless --skip-transformations is also set"));
    auto const skip_transformations =
        cl::opt<bool>(
            "skip-transformations",
            cl::init(false),
            cl::cat(extraCat),
            cl::desc("Skip the transformations pipeline stage, for example beta-delta normalization"));

    auto const input_files = cl::list<std::string>(cl::Positional, cl::desc("<input-files>"));

    cl::ParseCommandLineOptions(
        argc, argv,
        "dep0 - DepC Bootstrapping Compiler",
        /*Errs*/ nullptr,
        /*EnvVar*/ nullptr,
        /*LongOptionsUseDoubleDash*/ true);

    if (input_files.empty())
        return failure("no input files");

    auto const input_file_paths = std::vector<fs::path>(input_files.begin(), input_files.end());

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

    auto const file_type = llvm::codegen::getExplicitFileType().getValueOr(llvm::CGFT_ObjectFile);
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
            .out_file_name = out_file_name.empty() ? std::nullopt : std::optional<fs::path>{out_file_name.getValue()},
            .no_prelude =  no_prelude,
            .skip_transformations = skip_transformations,
            .unverified = emit_llvm_unverified
        }});
    if (compile_and_assemble or compile_only or file_type == llvm::CGFT_AssemblyFile)
        return run(job_t{job_t::compile_only_t{
            .input_files = input_file_paths,
            .out_file_name = out_file_name.empty() ? std::nullopt : std::optional<fs::path>{out_file_name.getValue()},
            .no_prelude = no_prelude,
            .skip_transformations = skip_transformations,
            .machine = std::ref(*machine),
            .file_type = compile_and_assemble ? llvm::CGFT_ObjectFile : llvm::CGFT_AssemblyFile
        }});
    return run(job_t{job_t::compile_and_link_t{
        .input_files = input_file_paths,
        .out_file_name = out_file_name.empty() ? fs::path("a.out") : fs::path(out_file_name.getValue()),
        .no_prelude = no_prelude,
        .skip_transformations = skip_transformations,
        .machine = std::ref(*machine)
    }});
}
