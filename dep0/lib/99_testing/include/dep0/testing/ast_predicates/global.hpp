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

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P>
boost::test_tools::predicate_result is_global(ast::expr_t<P> const& expr, std::string_view const name)
{
    auto const g = std::get_if<typename ast::expr_t<P>::global_t>(&expr.value);
    if (not g)
        return failure("expression is not global_t but ", pretty_name(expr.value));
    auto const s = [&]
    {
        std::ostringstream buf;
        if (g->module_name)
            buf << *g->module_name << "::";
        buf << g->name;
        return buf.str();
    }();
    if (s != name)
    {
        auto failed = boost::test_tools::predicate_result(false);
        failed.message().stream() << s << " != " << name;
        return failed;
    }
    return true;
}

inline auto global(std::string const& name)
{
    return [name] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_global(expr, name);
    };
}

} // namespace dep0::testing
