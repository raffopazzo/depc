#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/pretty_print.hpp"

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include <cassert>

namespace dep0::testing {

namespace detail {

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

template <std::size_t I, typename T, typename... Predicates>
boost::test_tools::predicate_result check_all_tuple(std::vector<T> const& v, std::tuple<Predicates...> const& fs)
{
    if constexpr (I < sizeof...(Predicates))
    {
        if (auto const result = std::get<I>(fs)(v.at(I)); not result)
            return failure("predicate at index ", I, ": ", result.message());
        return check_all_tuple<I+1>(v, fs);
    }
    else
        return true;
}
} // namespace detail

// sort_t predicates

template <ast::Properties P, Predicate<ast::type_t<P>> F>
boost::test_tools::predicate_result is_type_of(ast::sort_t<P> const& sort, F&& f)
{
    if (auto const t = std::get_if<ast::type_t<P>>(&sort))
        return std::forward<F>(f)(*t);
    else
        return failure("sort is not type but ", pretty_name(sort));
}

// type_t predicates

template <ast::Properties P>
boost::test_tools::predicate_result is_type_var(ast::type_t<P> const& type, std::string_view const name)
{
    auto const var = std::get_if<typename ast::type_t<P>::var_t>(&type.value);
    if (not var)
        return failure("type is not var_t but ", pretty_name(type.value));
    if (var->name.txt != name or var->name.idx != 0ul)
    {
        auto failed = boost::test_tools::predicate_result(false);
        pretty_print(failed.message().stream(), var->name) << " != " << name;
        return failed;
    }
    return true;
}

template <ast::Properties P>
boost::test_tools::predicate_result
is_type_binder(typename ast::type_t<P>::arr_t::arg_type_t const& x, std::string_view const name)
{
    auto const var = std::get_if<typename ast::type_t<P>::var_t>(&x);
    if (not var)
        return failure("function argument is not type_t::var_t but ", pretty_name(x));
    if (var->name.txt != name or var->name.idx != 0ul)
    {
        auto failed = boost::test_tools::predicate_result(false);
        pretty_print(failed.message().stream(), var->name) << " != " << name;
        return failed;
    }
    return true;
}

template <ast::Properties P, Predicate<ast::type_t<P>> F>
boost::test_tools::predicate_result
is_arg_of_type(typename ast::type_t<P>::arr_t::arg_type_t const& x, F&& f)
{
    auto const type = std::get_if<ast::type_t<P>>(&x);
    if (not type)
        return failure("function argument is not type_t but ", pretty_name(x));
    return std::forward<F>(f)(*type);
}

template <ast::Properties P, typename... ArgPredicates, Predicate<typename ast::type_t<P>> F>
boost::test_tools::predicate_result
is_arr_of(ast::type_t<P> const& type, std::tuple<ArgPredicates...> const& f_arg_types, F&& f_ret_type)
{
    auto const arr = std::get_if<typename ast::type_t<P>::arr_t>(&type.value);
    if (not arr)
        return failure("type is not arr_t but ", pretty_name(type.value));
    if (auto const result = std::forward<F>(f_ret_type)(arr->ret_type.get()); not result)
        return failure("return type predicate failed: ", result.message());
    if (arr->arg_types.size() != sizeof...(ArgPredicates))
        return failure("wrong number of arguments: ", sizeof...(ArgPredicates), " != ", arr->arg_types.size());
    return detail::check_all_tuple<0ul>(arr->arg_types, f_arg_types);
}

inline constexpr auto is_type_bool =
[] <ast::Properties P> (ast::type_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::type_t<P>::bool_t>(type.value))
        return true;
    else
        return failure("type is not bool_t but ", pretty_name(type.value));
};

inline constexpr auto is_type_i32 =
[] <ast::Properties P> (ast::type_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::type_t<P>::i32_t>(type.value))
        return true;
    else
        return failure("type is not i32_t but ", pretty_name(type.value));
};

inline constexpr auto is_type_u32 =
[] <ast::Properties P> (ast::type_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::type_t<P>::u32_t>(type.value))
        return true;
    else
        return failure("type is not u32_t but ", pretty_name(type.value));
};

// expr_t predicates

template <ast::Properties P>
boost::test_tools::predicate_result is_var(ast::expr_t<P> const& expr, std::string_view const name)
{
    auto const var = std::get_if<typename ast::expr_t<P>::var_t>(&expr.value);
    if (not var)
        return failure("expression is not var_t but ", pretty_name(expr.value));
    if (var->name.txt != name or var->name.idx != 0ul)
    {
        auto failed = boost::test_tools::predicate_result(false);
        pretty_print(failed.message().stream(), var->name) << " != " << name;
        return failed;
    }
    return true;
}

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
boost::test_tools::predicate_result is_numeric_constant(ast::expr_t<P> const& expr, std::string_view const x)
{
    assert(x.size() > 0ul);
    auto const c = std::get_if<typename ast::expr_t<P>::numeric_constant_t>(&expr.value);
    if (not c)
        return failure("expression is not numeric_constant_t but ", pretty_name(expr.value));
    if (x[0] == '+' or x[0] == '-')
    {
        if (c->sign.value_or(0) == x[0] and c->number == x.substr(1))
            return true;
    }
    else
    {
        if (c->sign.has_value() == false and c->number == x)
            return true;
    }
    auto failed = boost::test_tools::predicate_result(false);
    failed.message().stream() << "numeric constant is not " << x << " but ";
    if (c->sign.has_value())
        failed.message().stream() << *c->sign;
    failed.message().stream() << c->number.view();
    return failed;
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

template <ast::Properties P, Predicate<ast::type_t<P>> F>
boost::test_tools::predicate_result is_type_expr_of(ast::expr_t<P> const& expr, F&& f)
{
    auto const t = std::get_if<typename ast::type_t<P>>(&expr.value);
    if (not t)
        return failure("expression is not type_t but ", pretty_name(expr.value));
    return std::forward<F>(f)(*t);
}

inline constexpr auto is_zero = [] <ast::Properties P> (ast::expr_t<P> const& expr)
{
    return is_numeric_constant(expr, "0");
};

// stmt_t predicates

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

// factories of type_t predicates

inline auto type_var(std::string const& name)
{
    return [name] <ast::Properties P> (ast::type_t<P> const& type)
    {
        return is_type_var(type, name);
    };
}

template <ast::Properties P> // compiler can't deduce P just by looking at `arg_type_t`, so need template here
inline auto type_binder(std::string const& name)
{
    return [name] (typename ast::type_t<P>::arr_t::arg_type_t const& x)
    {
        return is_type_binder<P>(x, name);
    };
}

template <ast::Properties P, Predicate<ast::type_t<P>> F>
inline auto arg_of_type(F&& f)
{
    return [f=std::forward<F>(f)] (typename ast::type_t<P>::arr_t::arg_type_t const& x)
    {
        return is_arg_of_type<P>(x, f);
    };
}

// factories of expr_t predicates

template <ast::Properties P, Predicate<ast::type_t<P>> F>
constexpr auto type_expr_of(F&& f)
{
    return [f=std::forward<F>(f)] (ast::expr_t<P> const& expr)
    {
        return is_type_expr_of(expr, f);
    };
}

inline auto var(std::string const& name)
{
    return [name] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_var(expr, name);
    };
}

inline auto boolean_constant(bool const value)
{
    return [value] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_boolean_constant(expr, value);
    };
}

inline auto numeric_constant(std::string const& value)
{
    return [value] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_numeric_constant(expr, value);
    };
}

} // namespace dep0::testing
