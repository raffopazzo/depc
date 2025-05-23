#pragma once

#include "dep0/ast/ast.hpp"

#include <optional>
#include <ranges>
#include <vector>

namespace dep0::ast {

/**
 * @brief Return the index of a field inside a struct or `nullopt` if none can be found.
 * @remarks This function is intended to be used when searching for a field addressed by a direct member access,
 * e.g. `x.foo` therefore fields that have been renamed (i.e. `index > 0`) are not considered.
 */
template <Properties P>
std::optional<std::size_t>
find_member_index(source_text const field_name, typename type_def_t<P>::struct_t const& s)
{
    for (auto const i: std::views::iota(0ul, s.fields.size()))
        if (s.fields[i].var.idx == 0ul and s.fields[i].var.name == field_name)
            return i;
    return {};
}

/**
 * @brief Return the iterator to a field inside a struct or the end iterator if none can be found.
 * @see @ref find_member_index
 */
template <Properties P>
typename std::vector<typename type_def_t<P>::struct_t::field_t>::const_iterator
find_member_field(source_text const field_name, typename type_def_t<P>::struct_t const& s)
{
    auto const i = find_member_index<P>(field_name, s);
    return i ? std::next(s.fields.cbegin(), *i) : s.fields.cend();
}

} // namespace dep0::ast
