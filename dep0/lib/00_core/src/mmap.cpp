/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include  "dep0/mmap.hpp"

#include <boost/iostreams/device/mapped_file.hpp>

namespace dep0 {

expected<source_text> mmap(std::filesystem::path const path)
{
    boost::iostreams::mapped_file_source file;
    try { file.open(path.native()); }
    catch (std::exception const& e) { return error_t("failed to mmap file", std::vector{error_t{e.what()}}); }
    if (file.is_open())
    {
        auto const text = std::string_view{file.begin(), file.size()};
        return source_text(make_source_handle<boost::iostreams::mapped_file_source>(std::move(file)), text);
    }
    else
        return error_t{"failed to mmap file wihtout reasons..."};
}

} // namespace dep0
