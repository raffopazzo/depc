#pragma once

#include "dep0/testing/ast_predicates/app.hpp"

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::stmt_t<P>>... F>
boost::test_tools::predicate_result check_body(ast::body_t<P> const& body, std::tuple<F...> f)
{
    auto constexpr N = sizeof...(F);
    if (auto const n = body.stmts.size(); n != N)
        return failure("wrong number of statements: ", N, " != ", n);
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<Is>(f)(body.stmts[Is]); not tmp)
                    result = failure("statement ", Is, ": ", tmp.message());
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F, Predicate<ast::expr_t<P>>... ArgPredicates>
boost::test_tools::predicate_result is_func_call_of(ast::stmt_t<P> const& stmt, F&& f_func, ArgPredicates&&... f_args)
{
    auto const app = std::get_if<typename ast::expr_t<P>::app_t>(&stmt.value);
    if (not app)
        return failure("statement is not a function call but ", pretty_name(stmt.value));
    if (auto const r = impl::is_app_of<P>(*app, std::forward<F>(f_func), std::forward<ArgPredicates>(f_args)...); not r)
        return failure("inside function call: ", r.message());
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
        return failure("inside if-else condition: ", result.message());
    if (if_->false_branch.has_value())
        return failure("if-else has false branch but it should not");
    if (auto const result = check_body(if_->true_branch, std::move(f_then)); not result)
        return failure("inside true branch: ", result.message());
    return true;
}

template <
    ast::Properties P,
    Predicate<ast::expr_t<P>> F_cond,
    Predicate<ast::stmt_t<P>>... F_then>
constexpr auto if_else_of(F_cond&& f_cond, std::tuple<F_then...> f_then)
{
    return
        [f_cond=std::forward<F_cond>(f_cond), f_then=std::move(f_then)]
        (ast::stmt_t<P> const& stmt)
        {
            return is_if_else(stmt, f_cond, f_then);
        };
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
        return failure("inside if-else condition: ", result.message());
    if (not if_->false_branch.has_value())
        return failure("if-else should have false branch but it does not");
    if (auto const result = check_body(if_->true_branch, std::move(f_then)); not result)
        return failure("inside true branch: ", result.message());
    if (auto const result = check_body(*if_->false_branch, std::move(f_else)); not result)
        return failure("inside false branch: ", result.message());
    return true;
}

template <
    ast::Properties P,
    Predicate<ast::expr_t<P>> F_cond,
    Predicate<ast::stmt_t<P>>... F_then,
    Predicate<ast::stmt_t<P>>... F_else>
constexpr auto if_else_of(F_cond&& f_cond, std::tuple<F_then...> f_then, std::tuple<F_else...> f_else)
{
    return
        [f_cond=std::forward<F_cond>(f_cond), f_then=std::move(f_then), f_else=std::move(f_else)]
        (ast::stmt_t<P> const& stmt)
        {
            return is_if_else(stmt, f_cond, f_then, f_else);
        };
}

template <ast::Properties P>
boost::test_tools::predicate_result is_return_of_void(ast::stmt_t<P> const& stmt)
{
    auto const ret = std::get_if<typename ast::stmt_t<P>::return_t>(&stmt.value);
    if (not ret)
        return failure("statement is not return but ", pretty_name(stmt.value));
    if (ret->expr.has_value())
        return failure("return statement contains an expression but it should not");
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
    if (auto const result = std::forward<F>(f)(*ret->expr); not result)
        return failure("inside return expression: ", result.message());
    return true;
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
