#include "dep0/linker/link.hpp"

#include "private/x86_64_linux.hpp"

namespace dep0::linker {

expected<std::true_type> link(
    std::vector<std::string> const& object_files,
    llvm::Triple const target_triple,
    llvm::Triple const host_triple,
    std::string_view const output_file_name
) noexcept
{
    using enum llvm::Triple::ArchType;
    using enum llvm::Triple::OSType;
    if (target_triple != host_triple)
        return error_t("cross-compilation not yet supported");
    auto const arch_os = std::pair{target_triple.getArch(), target_triple.getOS()};
    if (arch_os == std::pair{x86_64, Linux})
        return x86_64_linux::link(object_files, output_file_name);
    return error_t("target triple not yet supported");
}

} // namespace dep0::linker
