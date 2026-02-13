/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Contains the definition of `dep0::typecheck::location_map_t`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"

#include <map>

namespace dep0::typecheck {

/** @brief Helper type to keep track of which variables refer to the same location as other variables. */
struct location_map_t
{
    std::map<expr_t::var_t, expr_t::var_t> map;

    /** @brief Constructs a new map formed by the union of the two input maps. */
    static expected<location_map_t> combine(location_map_t const&, location_map_t const&);
};

} // namespace dep0::typecheck
