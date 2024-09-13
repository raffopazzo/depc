#include "dep0/compile/compile.hpp"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ToolOutputFile.h>

#include <sstream>

namespace dep0::compile {

static
expected<temp_file_t>
compile(
    llvm::Module& module,
    llvm::TargetMachine& machine,
    llvm::CodeGenFileType const output_file_type)
{
    auto temp_file =
        make_temp_file(
            output_file_type == llvm::CodeGenFileType::CGFT_AssemblyFile
            ? temp_file_open_mode_t::text
            : temp_file_open_mode_t::binary);
    if (not temp_file)
        return temp_file.error();
    llvm::raw_fd_ostream ostream(temp_file->fd(), /*shouldClose*/ false);
    llvm::legacy::PassManager pass_manager;
    if (machine.addPassesToEmitFile(pass_manager, ostream, nullptr, output_file_type))
        return dep0::error_t("cannot emit the desired file type for the target machine");
    pass_manager.run(module);
    return temp_file;
}

expected<temp_file_t> compile_only(llvm::Module& module, llvm::TargetMachine& machine)
{
    return compile(module, machine, llvm::CGFT_AssemblyFile);
}

expected<temp_file_t> compile_and_assemble(llvm::Module& module, llvm::TargetMachine& machine)
{
    return compile(module, machine, llvm::CGFT_ObjectFile);
}

} // namespace dep0::compile
