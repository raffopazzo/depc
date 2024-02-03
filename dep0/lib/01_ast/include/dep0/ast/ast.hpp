#pragma once

#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

#include <boost/multiprecision/cpp_int.hpp>
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
template <Properties P> struct func_arg_t;
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
};

template <Properties P>
struct expr_t
{
    using rec_t = boost::recursive_wrapper<expr_t>;
    using properties_t = typename P::expr_properties_type;
    using body_t = ast::body_t<P>;

    struct typename_t {};
    struct bool_t {};
    struct unit_t {};
    struct i8_t {};
    struct i16_t {};
    struct i32_t {};
    struct i64_t {};
    struct u8_t {};
    struct u16_t {};
    struct u32_t {};
    struct u64_t {};
    struct boolean_constant_t
    {
        bool value;
    };
    struct numeric_constant_t
    {
        boost::multiprecision::cpp_int value;
    };
    struct boolean_expr_t
    {
        struct gt_t
        {
            rec_t lhs;
            rec_t rhs;
        };
        struct gte_t
        {
            rec_t lhs;
            rec_t rhs;
        };
        struct lt_t
        {
            rec_t lhs;
            rec_t rhs;
        };
        struct lte_t
        {
            rec_t lhs;
            rec_t rhs;
        };
        using value_t = std::variant<gt_t, gte_t, lt_t, lte_t>;
        value_t value;
    };
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
    struct var_t
    {
        source_text name;
        std::size_t idx = 0ul; // >0 if renaming occurred (eg during substitution or alpha-conversion)
        bool operator<(var_t const& that) const { return std::tie(name, idx) < std::tie(that.name, that.idx); }
        bool operator==(var_t const&) const = default;
    };
    struct app_t
    {
        rec_t func;
        std::vector<expr_t> args;
    };
    struct abs_t
    {
        std::vector<func_arg_t<P>> args;
        rec_t ret_type;
        body_t body;
    };
    struct pi_t
    {
        std::vector<func_arg_t<P>> args;
        rec_t ret_type;
    };
    struct array_t {};
    struct init_list_t
    {
        std::vector<expr_t> values;
    };
    struct subscript_t
    {
        rec_t array;
        rec_t index;
    };

    using value_t =
        std::variant<
            typename_t,
            bool_t, unit_t, i8_t, i16_t, i32_t, i64_t, u8_t, u16_t, u32_t, u64_t,
            boolean_constant_t, numeric_constant_t, boolean_expr_t, arith_expr_t,
            var_t, app_t, abs_t, pi_t,
            array_t, init_list_t, subscript_t
        >;

    properties_t properties;
    value_t value;
};

template <Properties P>
typename expr_t<P>::app_t const* get_if_app_of_array(expr_t<P> const& x)
{
    auto const app = std::get_if<typename expr_t<P>::app_t>(&x.value);
    return app and std::holds_alternative<typename expr_t<P>::array_t>(app->func.get().value) ? app : nullptr;
}

template <Properties P>
struct func_arg_t
{
    using properties_t = typename P::func_arg_properties_type;
    using expr_t = ast::expr_t<P>;

    properties_t properties;
    expr_t type;
    std::optional<typename expr_t::var_t> var;
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
    };
    struct return_t
    {
        std::optional<expr_t> expr;
    };
    using value_t = std::variant<typename expr_t::app_t, if_else_t, return_t>;

    properties_t properties;
    value_t value;
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
        std::optional<boost::multiprecision::cpp_int> max_abs_value;
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
};

} // namespace dep0::ast
