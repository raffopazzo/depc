/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
 #pragma once

#include <type_traits>

namespace dep0 {

enum class const_t { no, yes };

template <const_t Const, typename T>
using maybe_const_ref = std::conditional_t<Const == const_t::yes, T const&, T&>;

} // namespace dep0
