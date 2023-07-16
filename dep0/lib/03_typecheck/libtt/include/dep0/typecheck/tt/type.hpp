#pragma once

#include <boost/variant/recursive_wrapper.hpp>

#include "dep0/source.hpp"

#include <compare>
#include <ostream>
#include <variant>

namespace dep0::typecheck::tt {

struct type_t
{
    using rec_t = boost::recursive_wrapper<type_t>;

    struct var_t
    {
        source_text name;

        auto operator<=>(var_t const&) const = default;
    };

    struct arr_t
    {
        rec_t dom;
        rec_t img;

        bool operator==(arr_t const&) const;
    };

    using value_t = std::variant<var_t, arr_t>;

    value_t value;

    bool operator==(type_t const&) const = default;

    static type_t var(source_text);
    static type_t arr(type_t, type_t);
};

inline bool is_var(type_t const& x) { return std::holds_alternative<type_t::var_t>(x.value); }
inline bool is_arr(type_t const& x) { return std::holds_alternative<type_t::arr_t>(x.value); }

std::ostream& pretty_print(std::ostream&, type_t const&);

} // namespace dep0::typecheck::tt

