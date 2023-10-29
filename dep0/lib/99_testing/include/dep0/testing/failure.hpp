#pragma once

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <typename... Args>
boost::test_tools::predicate_result failure(Args&&... args)
{
    auto result = boost::test_tools::predicate_result(false);
    (result.message().stream() << ... << args);
    return result;
}

} // namespace dep0::testing
