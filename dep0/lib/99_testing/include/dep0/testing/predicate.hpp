/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <boost/test/tools/assertion_result.hpp>

#include <type_traits>

namespace dep0::testing {

template <typename Fn, typename Arg>
concept Predicate = std::is_invocable_r_v<boost::test_tools::predicate_result, Fn, Arg const&>;

} // namespace dep0::testing
