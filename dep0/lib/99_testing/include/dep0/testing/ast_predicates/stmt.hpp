#pragma once

#include "dep0/testing/check_all.hpp"
#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::expr_t<P>> F, Predicate<ast::expr_t<P>>... ArgPredicates>
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
        return check_all(app->args, std::forward<ArgPredicates>(f_args)...);
    else
        return true;
}

template <
    ast::Properties P,
    Predicate<ast::expr_t<P>> F_cond,
    Predicate<ast::stmt_t<P>>... F_then>
boost::test_tools::predicate_result
    is_if_else(ast::stmt_t<P> const& stmt, F_cond&& f_cond, std::tuple<F_then...> f_then)
{
    auto const if_ = std::get_if<typename ast::stmt_t<P>::if_else_t>(&stmt.value);
    if (not if_)
        return failure("statement is not if-else but ", pretty_name(stmt.value));
    if (auto const result = std::forward<F_cond>(f_cond)(if_->cond); not result)
        return failure("predicate has failed for if-else condition: ", result.message());
    if (if_->false_branch.has_value())
        return failure("if-else has false branch but it should not");
    auto constexpr N = sizeof...(F_then);
    if (auto const n = if_->true_branch.stmts.size(); n != N)
        return failure("wrong number of statements for the true branch: ", N, " != ", n);
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<Is>(f_then)(if_->true_branch.stmts[Is]); not tmp)
                    result = failure("argument predicate at index ", Is, " failed: ", tmp.message());
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

template <
    ast::Properties P,
    Predicate<ast::expr_t<P>> F_cond,
    Predicate<ast::stmt_t<P>>... F_then,
    Predicate<ast::stmt_t<P>>... F_else>
boost::test_tools::predicate_result
    is_if_else(ast::stmt_t<P> const& stmt, F_cond&& f_cond, std::tuple<F_then...> f_then, std::tuple<F_else...> f_else)
{
    auto const if_ = std::get_if<typename ast::stmt_t<P>::if_else_t>(&stmt.value);
    if (not if_)
        return failure("statement is not if-else but ", pretty_name(stmt.value));
    if (auto const result = std::forward<F_cond>(f_cond)(if_->cond); not result)
        return failure("predicate has failed for if-else condition: ", result.message());
    if (not if_->false_branch.has_value())
        return failure("if-else should have false branch but it does not");
    auto constexpr N_then = sizeof...(F_then);
    auto constexpr N_else = sizeof...(F_else);
    if (auto const n = if_->true_branch.stmts.size(); n != N_then)
        return failure("wrong number of statements for the true branch: ", N_then, " != ", n);
    if (auto const n = if_->false_branch->stmts.size(); n != N_else)
        return failure("wrong number of statements for the false branch: ", N_else, " != ", n);
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<Is>(f_then)(if_->true_branch.stmts[Is]); not tmp)
                    result = failure("argument predicate at index ", Is, " failed: ", tmp.message());
        }(), ...);
    } (std::make_index_sequence<N_then>{});
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<Is>(f_else)(if_->false_branch->stmts[Is]); not tmp)
                    result = failure("argument predicate at index ", Is, " failed: ", tmp.message());
        }(), ...);
    } (std::make_index_sequence<N_else>{});
    return result;
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

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
constexpr auto return_of(F&& f)
{
    return [f=std::forward<F>(f)] (ast::stmt_t<P> const& x)
    {
        return is_return_of(x, f);
    };
}

} // namespace dep0::testing
