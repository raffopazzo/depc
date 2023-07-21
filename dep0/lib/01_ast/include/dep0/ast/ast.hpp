#pragma once

#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

#include <optional>
#include <vector>
#include <variant>

namespace dep0::ast {

// forward declarations

template <Properties P> struct module_t;
template <Properties P> struct type_def_t;
template <Properties P> struct func_def_t;
template <Properties P> struct type_t;
template <Properties P> struct body_t;
template <Properties P> struct stmt_t;
template <Properties P> struct expr_t;

// definitions

enum class sign_t { signed_v, unsigned_v };
enum class width_t { _8, _16, _32, _64 };

template <Properties P>
struct type_def_t
{
    struct integer_t
    {
        sign_t sign;
        width_t width;
        // signed integers are inside [-max_abs_value, +max_abs_value] whereas unsigned are inside [0, max_abs_value]
        source_text max_abs_value;
        bool operator==(integer_t const&) const = default;
    };

    using properties_t = typename P::type_def_properties_type;
    using value_t = std::variant<integer_t>;

    properties_t properties;
    source_text name;
    value_t value;

    bool operator==(type_def_t const&) const = default;
};

// TODO should this be renamed to `type_ref_t` (or even `type_expr_t`) ? If yes, what happens to bool/unit?
template <Properties P>
struct type_t
{
    using properties_t = typename P::type_properties_type;
    struct bool_t { bool operator==(bool_t const&) const { return true; } };
    struct unit_t { bool operator==(unit_t const&) const { return true; } };
    struct type_ref_t
    {
        source_text name;
        bool operator==(type_ref_t const&) const = default;
    };
    using value_t = std::variant<bool_t, unit_t, type_ref_t>;

    properties_t properties;
    value_t value;

    bool operator==(type_t const&) const = default;
};

template <Properties P>
struct expr_t
{
    using properties_t = typename P::expr_properties_type;
    struct boolean_constant_t
    {
        source_text value;
        bool operator==(boolean_constant_t const&) const = default;
    };
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
    using value_t = std::variant<boolean_constant_t, numeric_constant_t, fun_call_t>;

    properties_t properties;
    value_t value;

    bool operator==(expr_t const&) const = default;
};

template <Properties P>
struct stmt_t
{
    using properties_t = typename P::stmt_properties_type;
    using body_t = ast::body_t<P>;
    using expr_t = ast::expr_t<P>;
    struct fun_call_t
    {
        source_text name;
        bool operator==(fun_call_t const&) const = default;
    };
    struct if_else_t
    {
        expr_t cond;
        body_t true_branch;
        std::optional<body_t> false_branch;
        bool operator==(if_else_t const&) const = default;
    };
    struct return_t
    {
        std::optional<expr_t> expr;
        bool operator==(return_t const&) const = default;
    };
    using value_t = std::variant<fun_call_t, if_else_t, return_t>;

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
    using type_def_t = ast::type_def_t<P>;
    using func_def_t = ast::func_def_t<P>;

    properties_t properties;
    std::vector<type_def_t> type_defs;
    std::vector<func_def_t> func_defs;

    bool operator==(module_t const&) const = default;
};

} // namespace dep0::ast
