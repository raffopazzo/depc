/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/pretty_print.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing::details {

template <ast::Properties P>
boost::test_tools::predicate_result check_name(typename ast::expr_t<P>::var_t const& x, std::string_view const y)
{
    if (std::pair{x.name.view(), x.idx} != std::pair{y, 0ul})
    {
        auto failed = boost::test_tools::predicate_result(false);
        ast::pretty_print<P>(failed.message().stream(), x) << " != " << y;
        return failed;
    }
    return true;
}

} // namespace dep0::testing::details
