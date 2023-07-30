#pragma once

#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

#include <boost/variant/recursive_wrapper.hpp>

#include <optional>
#include <tuple>
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
struct type_t
{
    using rec_t = boost::recursive_wrapper<type_t>;
    using properties_t = typename P::type_properties_type;
    struct bool_t { bool operator==(bool_t const&) const { return true; } };
    struct unit_t { bool operator==(unit_t const&) const { return true; } };
    struct i8_t { bool operator==(i8_t const&) const { return true; } };
    struct i16_t { bool operator==(i16_t const&) const { return true; } };
    struct i32_t { bool operator==(i32_t const&) const { return true; } };
    struct i64_t { bool operator==(i64_t const&) const { return true; } };
    struct u8_t { bool operator==(u8_t const&) const { return true; } };
    struct u16_t { bool operator==(u16_t const&) const { return true; } };
    struct u32_t { bool operator==(u32_t const&) const { return true; } };
    struct u64_t { bool operator==(u64_t const&) const { return true; } };
    struct var_t
    {
        source_text name;
        bool operator==(var_t const&) const = default;
    };
    struct arr_t
    {
        // in lambda-2, an arrow can either introduce new type variables (pi-types) or refer to existing types
        std::vector<std::variant<var_t, type_t>> arg_types;
        rec_t ret_type;
        bool operator==(arr_t const& that) const
        {
            return std::tie(arg_types, ret_type.get()) == std::tie(that.arg_types, that.ret_type.get());
        }
    };
    using value_t =
        std::variant<
            bool_t, unit_t, i8_t, i16_t, i32_t, i64_t, u8_t, u16_t, u32_t, u64_t,
            var_t, arr_t>;

    properties_t properties;
    value_t value;

    bool operator==(type_t const&) const = default;
};

struct typename_t // TODO should be replaced with kind_t?
{
    bool operator==(typename_t const&) const { return true; }
};

template <Properties P>
using sort_t = std::variant<type_t<P>, typename_t>;

template <Properties P>
struct expr_t
{
    using rec_t = boost::recursive_wrapper<expr_t>;
    using properties_t = typename P::expr_properties_type;
    using body_t = ast::body_t<P>;
    using type_t = ast::type_t<P>;
    struct arith_expr_t
    {
        struct plus_t
        {
            rec_t lhs;
            rec_t rhs;
        };
        using value_t = std::variant<plus_t>;
        value_t value;
    };
    struct boolean_constant_t
    {
        source_text value;
        bool operator==(boolean_constant_t const&) const = default;
    };
    struct numeric_constant_t
    {
        std::optional<char> sign;
        source_text number;
        bool operator==(numeric_constant_t const&) const = default;
    };
    struct var_t
    {
        source_text name;
        bool operator==(var_t const&) const = default;
    };
    struct app_t
    {
        source_text name; // TODO should really be `expr_t` since you can invoke the result of another expression
        std::vector<expr_t> args;
        bool operator==(app_t const&) const = default;
    };
    struct abs_t
    {
        struct arg_t
        {
            sort_t<P> sort;
            source_text name;
            source_loc_t loc;
        };

        std::vector<arg_t> args;
        type_t ret_type;
        body_t body;
    };

    using value_t =
        std::variant<
            arith_expr_t,
            boolean_constant_t,
            numeric_constant_t,
            var_t,
            app_t,
            abs_t,
            type_t>;

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
    using value_t = std::variant<typename expr_t::app_t, if_else_t, return_t>;

    properties_t properties;
    value_t value;

    bool operator==(stmt_t const&) const = default;
};

enum class sign_t { signed_v, unsigned_v };
enum class width_t { _8, _16, _32, _64 };

template <Properties P>
struct type_def_t
{
    struct integer_t
    {
        source_text name;
        sign_t sign;
        width_t width;
        std::optional<source_text> max_abs_value;
    };

    using properties_t = typename P::type_def_properties_type;
    using value_t = std::variant<integer_t>;
    properties_t properties;
    value_t value;
};

template <Properties P>
struct func_def_t
{
    using properties_t = typename P::func_def_properties_type;

    properties_t properties;
    source_text name;
    expr_t<P>::abs_t value;

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
