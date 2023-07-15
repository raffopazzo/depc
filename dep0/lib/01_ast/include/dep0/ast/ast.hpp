#pragma once

#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

#include <optional>
#include <vector>
#include <variant>

namespace dep0::ast {

// forward declarations

template <Properties P> struct module_t;
template <Properties P> struct func_def_t;
template <Properties P> struct type_t;
template <Properties P> struct body_t;
template <Properties P> struct stmt_t;
template <Properties P> struct expr_t;

// definitions

template <Properties P>
struct type_t
{
    using properties_t = typename P::type_properties_type;
    struct unit_t { bool operator==(unit_t const&) const { return true; } };
    struct int_t { bool operator==(int_t const&) const { return true; } };
    using value_t = std::variant<unit_t, int_t>;

    properties_t properties;
    value_t value;

    bool operator==(type_t const&) const = default;
};

template <Properties P>
struct expr_t
{
    using properties_t = typename P::expr_properties_type;
    struct numeric_constant_t
    {
        source_text number;
        bool operator==(numeric_constant_t const&) const = default;
    };
    struct fun_call_t
    {
        source_text name;
        bool operator==(fun_call_t const&) const = default;
    };
    using value_t = std::variant<numeric_constant_t, fun_call_t>;

    properties_t properties;
    value_t value;

    bool operator==(expr_t const&) const = default;
};

template <Properties P>
struct stmt_t
{
    using properties_t = typename P::stmt_properties_type;
    using expr_t = ast::expr_t<P>;
    struct return_t
    {
        std::optional<expr_t> expr;
        bool operator==(return_t const&) const = default;
    };
    using value_t = std::variant<return_t>;

    properties_t properties;
    value_t value;

    bool operator==(stmt_t const&) const = default;
};

template <Properties P>
struct body_t
{
    using properties_t = typename P::body_properties_type;
    using stmt_t = ast::stmt_t<P>;

    properties_t properties;
    std::vector<stmt_t> stmts;

    bool operator==(body_t const&) const = default;
};

template <Properties P>
struct func_def_t
{
    using properties_t = typename P::func_def_properties_type;
    using type_t = ast::type_t<P>;
    using body_t = ast::body_t<P>;

    properties_t properties;
    type_t type;
    source_text name;
    body_t body;

    bool operator==(func_def_t const&) const = default;
};

template <Properties P>
struct module_t
{
    using properties_t = typename P::module_properties_type;
    using func_def_t = ast::func_def_t<P>;

    properties_t properties;
    std::vector<func_def_t> func_defs;

    bool operator==(module_t const&) const = default;
};

} // namespace dep0::ast
