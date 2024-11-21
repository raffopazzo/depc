/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/transform/transform.hpp"

#include "dep0/typecheck/ast.hpp"

namespace dep0::transform {

struct beta_delta_normalization_t
{
    expected<std::true_type> operator()(typecheck::module_t&) const;
};
static_assert(Transform<beta_delta_normalization_t>);

} // namespace dep0::transform
