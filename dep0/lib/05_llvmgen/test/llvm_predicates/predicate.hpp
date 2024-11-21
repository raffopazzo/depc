/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/testing/predicate.hpp"

namespace dep0::llvmgen::testing {

template <typename Fn, typename Arg>
concept Predicate = dep0::testing::Predicate<Fn, Arg>;

} // namespace dep0::llvmgen::testing
