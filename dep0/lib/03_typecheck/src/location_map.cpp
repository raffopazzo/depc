/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/location_map.hpp"

#include "dep0/ast/pretty_print.hpp"

#include <vector>
#include <sstream>

namespace dep0::typecheck {
expected<location_map_t> location_map_t::combine(location_map_t const& a, location_map_t const& b)
{
    auto result = a;
    std::vector<error_t> reasons;
    for (auto const& [v1, v2]: b.map)
    {
        auto const [it, inserted] = result.map.try_emplace(v1, v2);
        if (not inserted and it->second != v2)
        {
            std::ostringstream err;
            pretty_print<properties_t>(err << '`', v1) << " ` canot refer both to ";
            pretty_print<properties_t>(err << '`', it->second) << "` and ";
            pretty_print<properties_t>(err << '`', v2) << '`';
            reasons.push_back(error_t(err.str()));
        }
    }
    if (not reasons.empty())
        return error_t("failed to combine location maps", std::move(reasons));
    return std::move(result);
}

} // namespace dep0::typecheck
