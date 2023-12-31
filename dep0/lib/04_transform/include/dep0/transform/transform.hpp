#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"

#include <concepts>
#include <functional>
#include <type_traits>

namespace dep0::transform {

template <typename F>
concept Transform = std::is_invocable_r_v<expected<std::true_type>, F, typecheck::module_t&>;

using transform_t = std::function<expected<std::true_type>(typecheck::module_t&)>;
static_assert(Transform<transform_t>);

} // namespace dep0::transform
