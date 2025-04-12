#pragma once

#include <type_traits>

namespace dep0 {

enum class const_t { no, yes };

template <const_t Const, typename T>
using maybe_const_ref = std::conditional_t<Const == const_t::yes, T const&, T&>;

} // namespace dep0
