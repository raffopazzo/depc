#include "dep0/mmap.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/environment.hpp"
#include "dep0/transform/run.hpp"
#include "dep0/transform/beta_delta_normalization.hpp"
#include "dep0/llvmgen/gen.hpp"

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

#include <iostream>

static llvm::codegen::RegisterCodeGenFlags WTF;

static int failure(std::string_view const msg)
{
    llvm::WithColor::error(llvm::errs()) << msg << '\n';
    return 1;
}
static int failure(std::string_view const file_name, std::string_view const msg)
{
    llvm::WithColor::error(llvm::errs(), file_name) << msg << '\n';
    return 1;
}

static int failure(std::string_view const file_name, std::string_view const msg, dep0::error_t const& error)
{
    std::ostringstream str;
    dep0::pretty_print(str, error);
    llvm::WithColor::error(llvm::errs(), file_name) << msg << ": " << str.str() << '\n';
    return 1;
}

static int failure(std::string_view const file_name, std::string_view const msg, dep0::typecheck::error_t const& error)
{
    std::ostringstream str;
    dep0::typecheck::pretty_print(str, error);
    llvm::WithColor::error(llvm::errs(), file_name) << msg << ": " << str.str() << '\n';
    return 1;
}

int main(int argc, char** argv)
{
    namespace cl = llvm::cl;
    auto const emit_llvm =
        cl::opt<bool>(
            "emit-llvm",
            cl::desc("Emit IR code instead of assembler"),
            cl::init(false));
    auto const input_files =
        cl::list<std::string>(
            cl::Positional,
            cl::desc("<input-files>"));
    auto const mtriple =
        cl::opt<std::string>(
            "mtriple",
            cl::desc("Override target triple for module"));
    auto const no_prelude =
        cl::opt<bool>(
            "no-prelude",
            cl::desc("Do not pre-import the prelude module"),
            cl::init(false));
    auto const print_ast =
        cl::opt<bool>(
            "print-ast",
            cl::desc("Print the AST before the codegen stage"),
            cl::init(false));
    auto const skip_transformations =
        cl::opt<bool>(
            "skip-transformations",
            cl::desc("Skip the transformations pipeline stage"),
            cl::init(false));
    auto typecheck_only =
        cl::opt<bool>(
            "typecheck-only",
            cl::desc("Stop after typechecking; don't emit any asm/obj code"),
            cl::init(false));
    auto const alias_t = cl::alias("t", cl::desc("alias for --typecheck-only"), cl::aliasopt(typecheck_only));
    auto unverified =
        cl::opt<bool>(
            "unverified",
            cl::desc("Emit unverified LLVM IR; only useful when debugging the llvmgen module"),
            cl::init(false));
    auto const alias_u = cl::alias("u", cl::desc("alias for --unverified"), cl::aliasopt(unverified));

    llvm::InitLLVM LLVM(argc, argv);
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();
    cl::ParseCommandLineOptions(argc, argv, "dep0 - DepC Bootstrapping Compiler");

    std::string err;
    auto target_triple =
        llvm::Triple(
            mtriple.empty()
            ? llvm::sys::getDefaultTargetTriple()
            : llvm::Triple::normalize(mtriple));
    llvm::Target const* const target = llvm::TargetRegistry::lookupTarget(llvm::codegen::getMArch(), target_triple, err);
    if (not target)
        return failure(err);
    llvm::TargetMachine* const machine =
        target->createTargetMachine(
            target_triple.getTriple(),
            llvm::codegen::getCPUStr(),
            llvm::codegen::getFeaturesStr(),
            llvm::codegen::InitTargetOptionsFromCodeGenFlags(target_triple),
            llvm::Optional<llvm::Reloc::Model>(llvm::codegen::getRelocModel()));
    if (not machine)
        return failure("failed to create target machine");
    llvm::LLVMContext llvm_context;
    auto const file_type = llvm::codegen::getFileType();
    auto const oflags =
        file_type == llvm::CodeGenFileType::CGFT_AssemblyFile
        ? llvm::sys::fs::OF_Text
        : llvm::sys::fs::OF_None;
    auto const suffix =
        file_type == llvm::CodeGenFileType::CGFT_AssemblyFile
        ? (emit_llvm ? ".ll" : ".s")
        : ".o";
    auto const base_env = no_prelude ? dep0::expected<dep0::typecheck::env_t>{} : dep0::typecheck::make_base_env();
    for (auto const& f: input_files)
    {
        if (not base_env)
            return failure(f, "prelude error", base_env.error());
        auto const parsed_module = dep0::parser::parse(f);
        if (not parsed_module)
            return failure(f, "parse error", parsed_module.error());
        auto typechecked_module = dep0::typecheck::check(*base_env, *parsed_module);
        if (not typechecked_module)
            return failure(f, "typecheck error", typechecked_module.error());
        if (typecheck_only or file_type == llvm::CodeGenFileType::CGFT_Null)
        {
            llvm::WithColor::note(llvm::outs(), f) << "typechecks correctly" << '\n';
            if (print_ast)
                dep0::ast::pretty_print(std::cout, *typechecked_module) << std::endl;
            continue;
        }
        if (not skip_transformations)
        {
            auto const transform_result =
                dep0::transform::run(
                    *typechecked_module,
                    dep0::transform::beta_delta_normalization_t{}
                );
            if (not transform_result)
            {
                std::ostringstream str;
                dep0::pretty_print(str, transform_result.error());
                llvm::WithColor::error(llvm::errs(), f) << "transformation error: " << str.str() << '\n';
            }
        }
        if (print_ast)
            dep0::ast::pretty_print(std::cout, *typechecked_module) << std::endl;
        auto llvm_module =
            dep0::llvmgen::gen(
                llvm_context,
                f,
                *typechecked_module,
                unverified ? dep0::llvmgen::verify_t::no : dep0::llvmgen::verify_t::yes);
        if (not llvm_module)
            return failure(f, "codegen error", llvm_module.error());
        std::error_code ec;
        auto out = llvm::ToolOutputFile(f + suffix, ec, oflags);
        llvm::legacy::PassManager pass_manager;
        if (file_type == llvm::CodeGenFileType::CGFT_AssemblyFile and emit_llvm)
            pass_manager.add(llvm::createPrintModulePass(out.os()));
        else if (machine->addPassesToEmitFile(pass_manager, out.os(), nullptr, file_type))
            return failure(f, "no support for file type");
        pass_manager.run(llvm_module->get());
        out.keep();
    }
    return 0;
}

