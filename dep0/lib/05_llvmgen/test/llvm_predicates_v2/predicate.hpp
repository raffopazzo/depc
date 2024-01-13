#pragma once

#include "dep0/testing/predicate.hpp"

namespace dep0::llvmgen::testing::v2 {

template <typename Fn, typename Arg>
concept Predicate = dep0::testing::Predicate<Fn, Arg>;

} // namespace dep0::llvmgen::testing::v2
