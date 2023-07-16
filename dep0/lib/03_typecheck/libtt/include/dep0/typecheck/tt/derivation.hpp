#pragma once

#include "dep0/typecheck/tt/context.hpp"
#include "dep0/typecheck/tt/term.hpp"
#include "dep0/typecheck/tt/type.hpp"

#include <boost/variant/recursive_wrapper.hpp>

#include <variant>

namespace dep0::typecheck::tt {

struct derivation_t
{
    using rec_t = boost::recursive_wrapper<derivation_t>;

    struct form_t
    {
        auto const& ty() const { return m_ty; }

        static form_t primitive_bool();
        static form_t primitive_int();
        static form_t primitive_unit();

        form_t(form_t const&) = default;
        form_t(form_t&&) = default;
        form_t& operator=(form_t const&) = default;
        form_t& operator=(form_t&&) = default;

    private:
        explicit form_t(type_t);
        type_t m_ty;
    };

    struct var_t
    {
        auto const& ctx() const { return m_ctx; }
        auto const& var() const { return m_var; }
        auto const& ty() const { return m_ty; }

        var_t(var_t const&) = default;
        var_t(var_t&&) = default;
        var_t& operator=(var_t const&) = default;
        var_t& operator=(var_t&&) = default;

    private:
        var_t(context_t, term_t::var_t, type_t);

        context_t m_ctx;
        term_t::var_t m_var;
        type_t m_ty;

        friend struct derivation_rules;
    };

    struct app_t
    {
        auto const& ctx() const { return m_ctx; }
        auto const& fun() const { return m_fun.get(); } // has a conclusion of arrow type
        auto const& arg() const { return m_arg.get(); } // has a conclusion whose type matches the domain of fun
        auto const& ty() const { return m_ty; }

        app_t(app_t const&) = default;
        app_t(app_t&&) = default;
        app_t& operator=(app_t const&) = default;
        app_t& operator=(app_t&&) = default;

    private:
        app_t(context_t, derivation_t, derivation_t, type_t ty);

        context_t m_ctx;
        rec_t m_fun;
        rec_t m_arg;
        type_t m_ty;

        friend struct derivation_rules;
    };

    struct abs_t
    {
        auto const& ctx() const { return m_ctx; }
        auto const& var() const { return m_var; }
        auto const& var_type() const { return m_var_type; }
        auto const& body() const { return m_body.get(); }
        auto const& ty() const { return m_ty; }

        abs_t(abs_t const&) = default;
        abs_t(abs_t&&) = default;
        abs_t& operator=(abs_t const&) = default;
        abs_t& operator=(abs_t&&) = default;

    private:
        abs_t(context_t, term_t::var_t, type_t, derivation_t, type_t);

        context_t m_ctx;
        term_t::var_t m_var;
        type_t m_var_type;
        rec_t m_body;
        type_t m_ty;

        friend struct derivation_rules;
    };

    using value_t = std::variant<form_t, var_t, app_t, abs_t>;
    value_t value;

    explicit derivation_t(form_t);
    explicit derivation_t(var_t);
    explicit derivation_t(app_t);
    explicit derivation_t(abs_t);
};

type_t const& type_of(derivation_t const&);

expected<derivation_t> type_assign(context_t const&, term_t const&);

}
