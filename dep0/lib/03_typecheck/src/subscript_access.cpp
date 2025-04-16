/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/subscript_access.hpp"

#include "dep0/ast/get_if_array.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

has_subscript_access_result_t has_subscript_access(expr_t const& type)
{
    if (auto const sigma = std::get_if<expr_t::sigma_t>(&type.value))
        return has_subscript_access_result::sigma_t{sigma->args};
    if (auto const arr = get_if_array(type))
        return has_subscript_access_result::array_t{arr->element_type.get(), arr->size.get()};
    if (auto const because = std::get_if<expr_t::because_t>(&type.value))
        return has_subscript_access(because->value.get());
    return has_subscript_access_result::no_t{};
}

} // namespace dep0::typecheck
