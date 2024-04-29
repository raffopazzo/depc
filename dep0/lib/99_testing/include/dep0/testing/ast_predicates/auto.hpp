#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

inline constexpr auto is_auto =
[] <ast::Properties P> (ast::expr_t<P> const& x)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::auto_t>(x.value))
        return true;
    else
        return failure("expression is not auto_t but ", pretty_name(x.value));
};

} // namespace dep0::testing
