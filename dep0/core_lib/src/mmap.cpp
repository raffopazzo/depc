#include  "dep0/mmap.hpp"

#include <boost/iostreams/device/mapped_file.hpp>

namespace dep0 {

expected<source_text> mmap(std::filesystem::path const path)
{
    boost::iostreams::mapped_file_source file;
    file.open(path.native());
    if (file.is_open())
    {
        auto const text = std::string_view{file.begin(), file.size()};
        return source_text{make_handle<boost::iostreams::mapped_file_source>(std::move(file)), text};
    }
    else
        return error_t{"Failed to mmap file"}; // TODO add more info
}

} // namespace dep0
