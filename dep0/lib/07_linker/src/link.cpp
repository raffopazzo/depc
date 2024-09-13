#include "dep0/linker/link.hpp"

#include "private/x86_64_linux.hpp"

namespace dep0::linker {

expected<temp_file_t> link(
    std::vector<std::filesystem::path> const& object_files,
    llvm::Triple const target,
    llvm::Triple const host
) noexcept
{
    using enum llvm::Triple::ArchType;
    using enum llvm::Triple::OSType;
    if (target != host)
        return error_t("cross-compilation not yet supported");
    auto const arch_os = std::pair{target.getArch(), target.getOS()};
    if (arch_os == std::pair{x86_64, Linux})
        return x86_64_linux::link(object_files);
    return error_t("target triple not yet supported");
}

} // namespace dep0::linker
