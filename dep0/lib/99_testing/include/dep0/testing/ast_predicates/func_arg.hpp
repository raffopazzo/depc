#pragma once

#include "dep0/testing/ast_predicates/details/check_name.hpp"

#include "dep0/testing/ast_predicates/types.hpp"

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
boost::test_tools::predicate_result is_arg(
    ast::func_arg_t<P> const& arg,
    F&& type_predicate,
    std::optional<std::string_view> const name,
    ast::qty_t const qty = ast::qty_t::many)
{
    if (arg.qty != qty)
    {
        auto const to_string = [] (ast::qty_t const qty)
        {
            switch (qty)
            {
            case ast::qty_t::zero: return "zero";
            case ast::qty_t::one: return "one";
            case ast::qty_t::many: return "many";
            default: return "<unknown>";
            }
        };
        return failure("argument quantity ", to_string(arg.qty), " != ", to_string(qty));
    }
    if (auto const result = std::forward<F>(type_predicate)(arg.type); not result)
        return failure("argument type predicate failed: ", result.message());
    if (name)
        return arg.var ? details::check_name<P>(*arg.var, *name) : failure("argument has no name");
    else if (arg.var)
        return failure("argument has a name but should be anonymous");
    else
        return true;
}

inline auto typename_(
    std::optional<std::string> name = std::nullopt,
    ast::qty_t const qty = ast::qty_t::many)
{
    return [name=std::move(name), qty] <ast::Properties P> (ast::func_arg_t<P> const& x)
    {
        return is_arg(x, is_typename, name, qty);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
inline auto arg_of(F&& f, std::optional<std::string> name = std::nullopt, ast::qty_t const qty = ast::qty_t::many)
{
    return [f=std::forward<F>(f), name=std::move(name), qty] (ast::func_arg_t<P> const& x)
    {
        return is_arg<P>(x, f, name, qty);
    };
}

} // namespace dep0::testing
