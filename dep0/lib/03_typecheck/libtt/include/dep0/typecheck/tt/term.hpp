#pragma once

#include "dep0/typecheck/tt/type.hpp"
#include "dep0/source.hpp"

#include <boost/variant/recursive_wrapper.hpp>

#include <variant>

namespace dep0::typecheck::tt {

struct term_t
{
    using rec_t = boost::recursive_wrapper<term_t>;

    struct const_t
    {
        std::size_t id;
        source_text value;
        bool operator<(const_t const& that) const = default;
        bool operator==(const_t const& that) const = default;
    };
    struct var_t
    {
        source_text name;

        bool operator<(var_t const& that) const;
        bool operator==(var_t const& that) const;
    };

    struct app_t
    {
        rec_t left;
        rec_t right;

        bool operator==(app_t const&) const;
    };

    struct abs_t
    {
        var_t var;
        type_t var_type;
        rec_t body;

        bool operator==(abs_t const&) const;
    };

    using value_t = std::variant<const_t, var_t, app_t, abs_t>;

    value_t value;

    bool operator==(term_t const&) const = default;

    static term_t const_(std::size_t, source_text);
    static term_t var(source_text);
    static term_t app(term_t, term_t);
    static term_t abs(source_text, type_t, term_t);
    static term_t abs(var_t, type_t, term_t);
};

inline bool is_var(term_t const& x) { return std::holds_alternative<term_t::var_t>(x.value); }
inline bool is_app(term_t const& x) { return std::holds_alternative<term_t::app_t>(x.value); }
inline bool is_abs(term_t const& x) { return std::holds_alternative<term_t::abs_t>(x.value); }

std::ostream& pretty_print(std::ostream&, term_t const&);

} // namespace dep0::typecheck::tt
