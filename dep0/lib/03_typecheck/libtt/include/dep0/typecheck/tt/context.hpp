#pragma once

#include "dep0/typecheck/tt/term.hpp"
#include "dep0/typecheck/tt/type.hpp"
#include "dep0/error.hpp"

#include <memory>
#include <optional>
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

public:
    context_t();
    context_t(context_t const&);
    context_t& operator=(context_t const&);
    context_t(context_t&&) = default;
    context_t& operator=(context_t&&) = default;
    ~context_t() = default;

    [[nodiscard]] expected<std::true_type> extend(term_t::var_t, type_t);
    std::optional<type_t> operator[](term_t::var_t) const;
};

} // namespace dep0::typecheck::tt
