/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::expr_t<P>> F_object>
boost::test_tools::predicate_result
is_member_of(ast::expr_t<P> const& expr, F_object&& f_object, std::string_view const name)
{
    auto const member = std::get_if<typename ast::expr_t<P>::member_t>(&expr.value);
    if (not member)
        return failure("type is not member_t but ", pretty_name(expr.value));
    if (auto const result = std::forward<F_object>(f_object)(member->object.get()); not result)
        return failure("inside object: ", result.message());
    if (member->field != name)
        return failure("accessed member name ", member->field, " != ", name);
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F_object>
constexpr auto member_of(F_object&& f_object, std::string_view const name)
{
    return [f_object=std::forward<F_object>(f_object), name=std::string(name)] (ast::expr_t<P> const& x)
    {
        return is_member_of(x, f_object, name);
    };
}

} // namespace dep0::testing
