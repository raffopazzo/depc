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
    struct state_t
    {
        std::shared_ptr<state_t const> parent;
        std::map<term_t::var_t, type_t> decls;

        explicit state_t(std::shared_ptr<state_t const>);
        static std::shared_ptr<state_t> inherit(std::shared_ptr<state_t const>);
    };

    std::shared_ptr<state_t> state;

    friend std::ostream& pretty_print(std::ostream&, context_t const&);

public:
    explicit context_t(std::shared_ptr<state_t const> parent = nullptr);
    context_t(context_t const&) = default;
    context_t(context_t&&) = default;
    context_t& operator=(context_t const&) = default;
    context_t& operator=(context_t&&) = default;

    context_t extend() const;

    [[nodiscard]] expected<std::true_type> add(term_t::var_t, type_t);
    std::optional<type_t> operator[](term_t::var_t) const;
};

std::ostream& pretty_print(std::ostream&, context_t const&);

} // namespace dep0::typecheck::tt
