#include "dep0/mmap.hpp"
#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
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

static llvm::codegen::RegisterCodeGenFlags WTF;

int main(int argc, char** argv)
{
    auto emit_llvm =
        llvm::cl::opt<bool>(
            "emit-llvm",
            llvm::cl::desc("Emit IR code instead of assembler"),
            llvm::cl::init(false));
    auto input_files =
        llvm::cl::list<std::string>(
            llvm::cl::Positional,
            llvm::cl::desc("<input-files>"));
    auto mtriple =
        llvm::cl::opt<std::string>(
            "mtriple",
            llvm::cl::desc("Override target triple for module"));

    llvm::InitLLVM LLVM(argc, argv);
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();
    llvm::cl::ParseCommandLineOptions(argc, argv, "dep0 - DepC Bootstrapping Compiler");

    std::string err;
    auto target_triple =
        llvm::Triple(
            mtriple.empty()
            ? llvm::sys::getDefaultTargetTriple()
            : llvm::Triple::normalize(mtriple));
    llvm::Target const* const target = llvm::TargetRegistry::lookupTarget(llvm::codegen::getMArch(), target_triple, err);
    if (not target)
    {
        llvm::WithColor::error(llvm::errs()) << err << '\n';
        return 1;
    }
    llvm::TargetMachine* const machine =
        target->createTargetMachine(
            target_triple.getTriple(),
            llvm::codegen::getCPUStr(),
            llvm::codegen::getFeaturesStr(),
            llvm::codegen::InitTargetOptionsFromCodeGenFlags(target_triple),
            llvm::Optional<llvm::Reloc::Model>(llvm::codegen::getRelocModel()));
    if (not machine)
    {
        llvm::WithColor::error(llvm::errs()) << "Failed to create target machine\n";
        return 1;
    }
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
    for (auto const& f: input_files)
    {
        auto parse_tree = dep0::parser::parse(f);
        if (not parse_tree)
        {
            std::ostringstream str;
            dep0::pretty_print(str, parse_tree.error());
            llvm::WithColor::error(llvm::errs(), f) << "Parse error: " << str.str() << '\n';
            return 1;
        }
        auto typechecked_tree = dep0::typecheck::check(*parse_tree);
        if (not typechecked_tree)
        {
            std::ostringstream str;
            dep0::pretty_print(str, typechecked_tree.error());
            llvm::WithColor::error(llvm::errs(), f) << "Typecheck error: " << str.str() << '\n';
            return 1;
        }
        auto llvm_module = dep0::llvmgen::gen(llvm_context, f, typechecked_tree->root);
        if (not llvm_module)
        {
            std::ostringstream str;
            dep0::pretty_print(str, llvm_module.error());
            llvm::WithColor::error(llvm::errs(), f) << "Codegen error: " << str.str() << '\n';
            return 1;
        }
        if (file_type == llvm::CodeGenFileType::CGFT_Null)
            continue;
        std::error_code ec;
        auto out = llvm::ToolOutputFile(f + suffix, ec, oflags);
        llvm::legacy::PassManager pass_manager;
        if (file_type == llvm::CodeGenFileType::CGFT_AssemblyFile and emit_llvm)
            pass_manager.add(llvm::createPrintModulePass(out.os()));
        else if (machine->addPassesToEmitFile(pass_manager, out.os(), nullptr, file_type))
        {
            llvm::WithColor::error(llvm::errs(), f) << "No support for file type\n";
            return 1;
        }
        pass_manager.run(llvm_module->get());
        out.keep();
    }
    return 0;
}

