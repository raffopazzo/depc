#include "dep0/digit_separator.hpp"

namespace dep0 {

bool contains_digit_separator(std::string_view const in)
{
    return in.find('\'') != std::string_view::npos;
}

std::string remove_digit_separator(std::string_view const in)
{
    std::string out;
    out.reserve(in.size());
    for (char const c: in)
        if (c != '\'')
            out.push_back(c);
    return out;
}
} // namespace dep0

