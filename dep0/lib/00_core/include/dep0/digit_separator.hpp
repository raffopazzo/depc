#pragma once

#include <string>
#include <string_view>

namespace dep0 {

bool contains_digit_separator(std::string_view);
void remove_digit_separator(std::string_view, std::string&);
std::string remove_digit_separator(std::string_view);

} // namespace dep0
