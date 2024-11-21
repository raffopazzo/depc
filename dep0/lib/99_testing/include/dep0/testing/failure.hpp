/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
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
