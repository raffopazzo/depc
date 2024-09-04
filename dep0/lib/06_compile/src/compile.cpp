#include "dep0/compile/compile.hpp"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ToolOutputFile.h>

#include <sstream>

namespace dep0::compile {

static
expected<std::filesystem::path>
compile(
    llvm::Module& module,
    llvm::TargetMachine& machine,
    llvm::CodeGenFileType const output_file_type,
    std::filesystem::path const& output_file_name)
{
    using enum llvm::sys::fs::OpenFlags;
    auto const oflags = output_file_type == llvm::CodeGenFileType::CGFT_AssemblyFile ? OF_Text : OF_None;
    std::error_code ec;
    auto out = llvm::ToolOutputFile(output_file_name.native(), ec, oflags);
    if (ec)
    {
        std::ostringstream err;
        err << "error opening `" << output_file_name << "`: " << ec.message();
        return dep0::error_t(err.str());
    }
    llvm::legacy::PassManager pass_manager;
    if (machine.addPassesToEmitFile(pass_manager, out.os(), nullptr, output_file_type))
        return dep0::error_t("cannot emit the desired file type for the target machine");
    pass_manager.run(module);
    out.keep();
    return output_file_name;
}

expected<std::filesystem::path>
compile_only(llvm::Module& module, llvm::TargetMachine& machine, std::filesystem::path const& output_file_name)
{
    return compile(module, machine, llvm::CGFT_AssemblyFile, output_file_name);
}

expected<std::filesystem::path>
compile_and_assemble(llvm::Module& module, llvm::TargetMachine& machine, std::filesystem::path const& output_file_name)
{
    return compile(module, machine, llvm::CGFT_ObjectFile, output_file_name);
}

} // namespace dep0::compile
