#pragma once

#include "dep0/typecheck/tt/term.hpp"
#include "dep0/typecheck/tt/type.hpp"
#include "dep0/error.hpp"

#include <memory>
#include <optional>
#include <ostream>
#include <map>

namespace dep0::typecheck::tt {

class context_t
{
public:
    struct type_decl_t
    {
        type_t::var_t subject;
        bool operator==(type_decl_t const&) const = default;
    };
    struct var_decl_t
    {
        term_t::var_t subject;
        type_t type;
        bool operator==(var_decl_t const&) const = default;
    };

    using decl_t = std::variant<type_decl_t, var_decl_t>;

    context_t();
    context_t(context_t const&) = default;
    context_t(context_t&&) = default;
    context_t& operator=(context_t const&) = default;
    context_t& operator=(context_t&&) = default;

    context_t extend() const;

    expected<std::true_type> add(type_t::var_t);
    expected<std::true_type> add(term_t::var_t, type_t);
    std::optional<type_decl_t> operator[](type_t::var_t const&) const;
    std::optional<var_decl_t> operator[](term_t::var_t const&) const;

private:
    struct state_t;

    explicit context_t(std::shared_ptr<state_t const> parent);

    friend std::ostream& pretty_print(std::ostream&, context_t const&);

    std::shared_ptr<state_t> state;
};

std::ostream& pretty_print(std::ostream&, context_t const&);

} // namespace dep0::typecheck::tt
