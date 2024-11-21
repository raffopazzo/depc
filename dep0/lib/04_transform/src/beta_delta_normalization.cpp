/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/transform/beta_delta_normalization.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

namespace dep0::transform {

expected<std::true_type> beta_delta_normalization_t::operator()(typecheck::module_t& m) const
{
    beta_delta_normalize(m);
    return std::true_type{};
}

} // namespace dep0::transform

