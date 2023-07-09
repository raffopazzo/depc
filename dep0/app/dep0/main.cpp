#include "dep0/mmap.hpp"
#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/llvmgen/gen.hpp"

#include <llvm/CodeGen/CommandFlags.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/WithColor.h>

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
    llvm::cl::ParseCommandLineOptions(argc, argv, "dep0 - DepC Bootstrapping Compiler");

    llvm::LLVMContext llvm_context;
    for (auto const& f: input_files)
    {
        auto parse_tree = dep0::parser::parse(f);
        if (not parse_tree)
        {
            std::ostringstream str;
            dep0::pretty_print(str, parse_tree.error());
            llvm::WithColor::error(llvm::errs(), f) << str.str() << '\n';
            return 1;
        }
        auto typechecked_tree = dep0::typecheck::check(*parse_tree);
        if (not typechecked_tree)
        {
            std::ostringstream str;
            dep0::pretty_print(str, typechecked_tree.error());
            llvm::WithColor::error(llvm::errs(), f) << str.str() << '\n';
            return 1;
        }
        auto llvm_module = dep0::llvmgen::gen(llvm_context, f, typechecked_tree->root);
        if (not llvm_module)
        {
            std::ostringstream str;
            dep0::pretty_print(str, llvm_module.error());
            llvm::WithColor::error(llvm::errs(), f) << str.str() << '\n';
            return 1;
        }
        llvm::WithColor::note(llvm::outs(), f) << "Should now emit the object file for " << f << '\n';
    }
    return 0;
}

