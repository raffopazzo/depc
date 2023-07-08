#pragma once

#include <any>
#include <cstdint>
#include <string_view>

namespace dep0 {

struct source_ref_t
{
    std::any ref;
};

struct source_loc_t
{
    std::size_t line;
    std::size_t col;
    std::string_view txt;

    source_loc_t(std::size_t const line, std::size_t const col, std::string_view const txt) :
        line(line), col(col), txt(txt)
    { }

    bool operator==(source_loc_t const&) const = default;
};

} // namespace dep0

