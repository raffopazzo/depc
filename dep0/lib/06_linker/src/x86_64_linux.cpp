#include "private/x86_64_linux.hpp"

#include <boost/process/v2/environment.hpp>
#include <boost/process/v2/execute.hpp>
#include <boost/process/v2/process.hpp>

#include <algorithm>
#include <iterator>

namespace dep0::linker::x86_64_linux {

expected<std::true_type> link(
    std::vector<std::string> const& object_files,
    std::string_view output_file_name
) noexcept
{
    auto const ld = boost::process::v2::environment::find_executable("ld");
    if (ld.empty())
        return error_t("linker not found");
    std::vector<std::string> args;
    args.push_back("-o");
    args.emplace_back(output_file_name);
    args.push_back("-dynamic-linker");
    args.push_back("/lib64/ld-linux-x86-64.so.2");
    args.push_back("/lib/x86_64-linux-gnu/crt1.o");
    std::ranges::copy(object_files, std::back_inserter(args));
    args.push_back("-lc");
    boost::asio::io_context ctx;
    boost::process::v2::process linker(ctx, ld, args);
    if (0 == linker.wait())
        return {};
    else
        return error_t("linker failed");
}

} // namespace dep0::linker::x86_64_linux
