#pragma once

#include <type_traits>

namespace dep0 {

template <bool Const, typename T>
using maybe_const_ref = std::conditional_t<Const, T const&, T&>;

} // namespace dep0
