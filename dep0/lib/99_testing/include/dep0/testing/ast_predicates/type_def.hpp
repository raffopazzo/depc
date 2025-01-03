/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"
#include "dep0/ast/pretty_print.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P>
boost::test_tools::predicate_result is_integer_def(
    ast::type_def_t<P> const& t,
    std::string_view const name,
    dep0::ast::sign_t const sign,
    dep0::ast::width_t const width)
{
    auto const integer = std::get_if<typename ast::type_def_t<P>::integer_t>(&t.value);
    if (not integer)
        return failure("typedef is not integer_t byt ", pretty_name(t.value));
    auto result = boost::test_tools::predicate_result(true);
    pretty_print(result.message().stream(), t);
    if (integer->name != name)
    {
        result = false;
        result.message().stream() << " has wrong name";
    }
    if (integer->sign != sign)
    {
        result = false;
        result.message().stream() << " has wrong sign";
    }
    if (integer->width != width)
    {
        result = false;
        result.message().stream() << " has wrong width";
    }
    return result;
}

} // namespace dep0::testing
