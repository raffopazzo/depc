#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/pretty_print.hpp"

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include <cassert>

namespace dep0::testing {

namespace detail {

template <ast::Properties P>
boost::test_tools::predicate_result check_name(typename ast::expr_t<P>::var_t const& x, std::string_view const y)
{
    if (std::pair{x.name.view(), x.idx} != std::pair{y, 0ul})
    {
        auto failed = boost::test_tools::predicate_result(false);
        pretty_print<P>(failed.message().stream(), x) << " != " << y;
        return failed;
    }
    return true;
}

template <std::size_t I, typename T, typename F, typename... Predicates>
boost::test_tools::predicate_result check_all(std::vector<T> const& v, F&& f, Predicates&&...fs)
{
    if (auto const result = std::forward<F>(f)(v.at(I)); not result)
        return failure("predicate at index ", I, ": ", result.message());
    if constexpr (sizeof...(Predicates) > 0ul)
        return check_all<I+1>(v, std::forward<Predicates>(fs)...);
    else
        return true;
}

} // namespace detail

// Predicates and factories are defined here, grouped by node type.
// Predicates come first, immediately followed by the corresponding factories, if any.

// type_def_t

template <ast::Properties P>
boost::test_tools::predicate_result is_integer_def(
    ast::type_def_t<P> const& t,
    std::string_view const name,
    dep0::ast::sign_t const sign,
    dep0::ast::width_t const width,
    std::optional<int> max_abs_value)
{
    auto const integer = std::get_if<typename ast::type_def_t<P>::integer_t>(&t.value);
    if (not integer)
        return failure("typedef is not integer_t byt ", pretty_name(t.value));
    auto result = boost::test_tools::predicate_result(true);
    pretty_print(result.message().stream(), t);
    if (integer->name != name)
    {
        result = false;
        result.message().stream() << " has wrong name";
    }
    if (integer->sign != sign)
    {
        result = false;
        result.message().stream() << " has wrong sign";
    }
    if (integer->width != width)
    {
        result = false;
        result.message().stream() << " has wrong width";
    }
    if (max_abs_value)
    {
        if (integer->max_abs_value != *max_abs_value)
        {
            result = false;
            result.message().stream() << " has wrong max abs value";
        }
    }
    else if (integer->max_abs_value)
    {
        result = false;
        result.message().stream() << " has a max abs value but it should not";
    }
    return result;
}

// expr_t

inline constexpr auto is_typename =
[] <ast::Properties P> (ast::expr_t<P> const& x)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::typename_t>(x.value))
        return true;
    else
        return failure("sort is not typename_t but ", pretty_name(x.value));
};

inline constexpr auto is_bool =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::bool_t>(type.value))
        return true;
    else
        return failure("type is not bool_t but ", pretty_name(type.value));
};

inline constexpr auto is_i32 =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::i32_t>(type.value))
        return true;
    else
        return failure("type is not i32_t but ", pretty_name(type.value));
};

inline constexpr auto is_u32 =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::u32_t>(type.value))
        return true;
    else
        return failure("type is not u32_t but ", pretty_name(type.value));
};

inline constexpr auto is_unit =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::unit_t>(type.value))
        return true;
    else
        return failure("type is not unit_t but ", pretty_name(type.value));
};

template <ast::Properties P>
boost::test_tools::predicate_result is_boolean_constant(ast::expr_t<P> const& expr, bool const x)
{
    auto const c = std::get_if<typename ast::expr_t<P>::boolean_constant_t>(&expr.value);
    if (not c)
        return failure("expression is not boolean_constant_t but ", pretty_name(expr.value));
    if (c->value != x)
        return failure("boolean constant is not ", x, " but ", c->value);
    return true;
}

template <ast::Properties P>
boost::test_tools::predicate_result is_numeric_constant(ast::expr_t<P> const& expr, int const x)
{
    auto const c = std::get_if<typename ast::expr_t<P>::numeric_constant_t>(&expr.value);
    if (not c)
        return failure("expression is not numeric_constant_t but ", pretty_name(expr.value));
    if (c->value != x)
        return failure("numeric constant ", c->value, " != ", x);
    return true;
}

inline auto constant(bool value)
{
    return [value] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_boolean_constant(expr, value);
    };
}

inline auto constant(int const value)
{
    return [value] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_numeric_constant(expr, value);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_plus(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::arith_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not arith_expr_t but ", pretty_name(expr.value));
    auto const* plus = std::get_if<typename ast::expr_t<P>::arith_expr_t::plus_t>(&x->value);
    if (not plus)
        return failure("arithmetic expression is not plus_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(plus->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(plus->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto plus(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_plus(x, f1, f2);
    };
}

template <ast::Properties P>
boost::test_tools::predicate_result is_var(ast::expr_t<P> const& expr, std::string_view const name)
{
    auto const var = std::get_if<typename ast::expr_t<P>::var_t>(&expr.value);
    if (not var)
        return failure("expression is not var_t but ", pretty_name(expr.value));
    return detail::check_name<P>(*var, name);
}

inline auto var(std::string const& name)
{
    return [name] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_var(expr, name);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F, typename... ArgPredicates>
boost::test_tools::predicate_result is_app_of(ast::expr_t<P> const& expr, F&& f_func, ArgPredicates&&... f_args)
{
    auto const app = std::get_if<typename ast::expr_t<P>::app_t>(&expr.value);
    if (not app)
        return failure("expression is not app_t but ", pretty_name(expr.value));
    if (auto const result = std::forward<F>(f_func)(app->func.get()); not result)
        return failure("predicate has failed for func: ", result.message());
    if (app->args.size() != sizeof...(ArgPredicates))
        return failure("wrong number of arguments ", app->args.size(), " != ", sizeof...(ArgPredicates));
    if constexpr (sizeof...(ArgPredicates) > 0ul)
        return detail::check_all<0ul>(app->args, std::forward<ArgPredicates>(f_args)...);
    else
        return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F, typename... ArgPredicates>
constexpr auto app_of(F&& f_func, ArgPredicates&&... f_args)
{
    return [f_func=std::forward<F>(f_func), ...f_args=std::forward<ArgPredicates>(f_args)] (ast::expr_t<P> const& x)
    {
        return is_app_of(x, f_func, f_args...);
    };
}

template <ast::Properties P, typename... ArgPredicates, Predicate<typename ast::expr_t<P>> F>
boost::test_tools::predicate_result
is_pi_of(ast::expr_t<P> const& type, std::tuple<ArgPredicates...> const& f_args, F&& f_ret_type)
{
    auto const pi = std::get_if<typename ast::expr_t<P>::pi_t>(&type.value);
    if (not pi)
        return failure("type is not pi_t but ", pretty_name(type.value));
    if (auto const result = std::forward<F>(f_ret_type)(pi->ret_type.get()); not result)
        return failure("return type predicate failed: ", result.message());
    auto constexpr N = sizeof...(ArgPredicates);
    if (pi->args.size() != N)
        return failure("wrong number of arguments: ", N, " != ", pi->args.size());
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<Is>(f_args)(pi->args[Is]); not tmp)
                    result = failure("argument predicate at index ", Is, " failed: ", tmp.message());
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F, typename... ArgPredicates>
constexpr auto pi_of(std::tuple<ArgPredicates...> args, F&& ret_type)
{
    return [args=std::move(args), ret_type=std::forward<F>(ret_type)] (ast::expr_t<P> const& x)
    {
        return is_pi_of(x, args, ret_type);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F_type, Predicate<ast::expr_t<P>> F_size>
boost::test_tools::predicate_result is_array_of(ast::expr_t<P> const& type, F_type&& f_type, F_size&& f_size)
{
    auto const array = std::get_if<typename ast::expr_t<P>::array_t>(&type.value);
    if (not array)
        return failure("type is not array_t but ", pretty_name(type.value));
    if (auto const result = std::forward<F_type>(f_type)(array->type.get()); not result)
        return failure("array element type predicate failed: ", result.message());
    if (auto const result = std::forward<F_size>(f_size)(array->size.get()); not result)
        return failure("array size predicate failed: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F_type, Predicate<ast::expr_t<P>> F_size>
constexpr auto array_of(F_type&& f_type, F_size&& f_size)
{
    return [f_type=std::forward<F_type>(f_type),f_size=std::forward<F_size>(f_size)] (ast::expr_t<P> const& x)
    {
        return is_array_of(x, f_type, f_size);
    };
}

template <ast::Properties P, typename... ValuePredicates>
boost::test_tools::predicate_result is_init_list_of(ast::expr_t<P> const& expr, ValuePredicates&&... f_values)
{
    auto const init_list = std::get_if<typename ast::expr_t<P>::init_list_t>(&expr.value);
    if (not init_list)
        return failure("expr is not init_list_t but ", pretty_name(expr.value));
    auto constexpr N = sizeof...(ValuePredicates);
    if (init_list->values.size() != N)
        return failure("wrong number of initializer values: ", N, " != ", init_list->values.size());
    if constexpr (sizeof...(ValuePredicates) > 0ul)
        return detail::check_all<0ul>(init_list->values, std::forward<ValuePredicates>(f_values)...);
    else
        return true;
}

template <typename... ValuePredicates>
constexpr auto init_list_of(ValuePredicates&&... f_values)
{
    return [...f_values=std::forward<ValuePredicates>(f_values)] <ast::Properties P> (ast::expr_t<P> const& x)
    {
        return is_init_list_of(x, f_values...);
    };
}

// func_arg_t

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
boost::test_tools::predicate_result is_arg(
    ast::func_arg_t<P> const& arg,
    F&& type_predicate,
    std::optional<std::string_view> const name)
{
    if (auto const result = std::forward<F>(type_predicate)(arg.type); not result)
        return failure("argument type predicate failed: ", result.message());
    if (name)
        return arg.var ? detail::check_name<P>(*arg.var, *name) : failure("argument has no name");
    else if (arg.var)
        return failure("argument has a name but should be anonymous");
    else
        return true;
}

inline auto typename_(std::optional<std::string> name = std::nullopt)
{
    return [name=std::move(name)] <ast::Properties P> (typename ast::func_arg_t<P> const& x)
    {
        return is_arg(x, is_typename, name);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
inline auto arg_of(F&& f, std::optional<std::string> name = std::nullopt)
{
    return [f=std::forward<F>(f), name=std::move(name)] (ast::func_arg_t<P> const& x)
    {
        return is_arg<P>(x, f, name);
    };
}

// stmt_t

template <ast::Properties P, Predicate<ast::expr_t<P>> F, typename... ArgPredicates>
boost::test_tools::predicate_result is_func_call_of(ast::stmt_t<P> const& stmt, F&& f_func, ArgPredicates&&... f_args)
{
    auto const app = std::get_if<typename ast::expr_t<P>::app_t>(&stmt.value);
    if (not app)
        return failure("statement is not a function call but ", pretty_name(stmt.value));
    if (auto const result = std::forward<F>(f_func)(app->func.get()); not result)
        return failure("predicate has failed for func: ", result.message());
    if (app->args.size() != sizeof...(ArgPredicates))
        return failure("wrong number of arguments ", app->args.size(), " != ", sizeof...(ArgPredicates));
    if constexpr (sizeof...(ArgPredicates) > 0ul)
        return detail::check_all<0ul>(app->args, std::forward<ArgPredicates>(f_args)...);
    else
        return true;
}

template <ast::Properties P, Predicate<typename ast::stmt_t<P>::if_else_t> F>
boost::test_tools::predicate_result is_if_else(ast::stmt_t<P> const& stmt, F&& f)
{
    auto const if_ = std::get_if<typename ast::stmt_t<P>::if_else_t>(&stmt.value);
    if (not if_)
        return failure("statement is not if-else but ", pretty_name(stmt.value));
    return std::forward<F>(f)(*if_);
}

template <ast::Properties P>
boost::test_tools::predicate_result is_return_of_void(ast::stmt_t<P> const& stmt)
{
    auto const ret = std::get_if<typename ast::stmt_t<P>::return_t>(&stmt.value);
    if (not ret)
        return failure("statement is not return but ", pretty_name(stmt.value));
    if (ret->expr.has_value())
        return failure("return statement contains an expression but it should not");
    else
        return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
boost::test_tools::predicate_result is_return_of(ast::stmt_t<P> const& stmt, F&& f)
{
    auto const ret = std::get_if<typename ast::stmt_t<P>::return_t>(&stmt.value);
    if (not ret)
        return failure("statement is not return but ", pretty_name(stmt.value));
    if (not ret->expr.has_value())
        return failure("return statement does not contain an expression");
    return std::forward<F>(f)(*ret->expr);
}

} // namespace dep0::testing
