/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/testing/ast_predicates/details/check_name.hpp"

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"
#include "dep0/ast/pretty_print.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P>
boost::test_tools::predicate_result is_integer_def(
    ast::type_def_t<P> const& t,
    std::string_view const name,
    dep0::ast::sign_t const sign,
    dep0::ast::width_t const width)
{
    auto const integer = std::get_if<typename ast::type_def_t<P>::integer_t>(&t.value);
    if (not integer)
        return failure("typedef is not integer_t but ", pretty_name(t.value));
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
    return result;
}

template <ast::Properties P>
boost::test_tools::predicate_result is_integer_def(
    typename ast::module_t<P>::entry_t const& x,
    std::string_view const name,
    dep0::ast::sign_t const sign,
    dep0::ast::width_t const width)
{
    auto const type_def = std::get_if<ast::type_def_t<P>>(&x);
    if (not type_def)
        return failure("entry is not a type_def_t but ", pretty_name(x));
    return is_integer_def(*type_def, name, sign, width);
}

template <ast::Properties P, Predicate<typename ast::type_def_t<P>::struct_t::field_t>... F>
boost::test_tools::predicate_result is_struct_def(ast::type_def_t<P> const& t, std::string_view const name, F&&... f)
{
    auto const s = std::get_if<typename ast::type_def_t<P>::struct_t>(&t.value);
    if (not s)
        return failure("typedef is not a struct_t but ", pretty_name(t.value));
    auto constexpr N = sizeof...(F);
    if (auto const n_fields = s->fields.size(); n_fields != N)
        return failure("wrong number of fields: ", N, " != ", n_fields);
    auto result = boost::test_tools::predicate_result(true);
    int next = 0;
    ([&]
    {
        if (result)
        {
            auto const i = next++;
            auto const& field = s->fields[i];
            if (auto const tmp = std::forward<F>(f)(s->fields[i]); not tmp)
            {
                result = failure("inside field ", i, ": ", tmp.message());
                return;
            }
        }
    }(), ...);
    return result;
}

template <ast::Properties P, Predicate<typename ast::type_def_t<P>::struct_t::field_t>... F>
boost::test_tools::predicate_result is_struct_def(
    typename ast::module_t<P>::entry_t const& x,
    std::string_view const name,
    F&&... f)
{
    auto const type_def = std::get_if<ast::type_def_t<P>>(&x);
    if (not type_def)
        return failure("entry is not a type_def_t but ", pretty_name(x));
    return is_struct_def(*type_def, name, std::forward<F>(f)...);
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
boost::test_tools::predicate_result
is_struct_field(typename ast::type_def_t<P>::struct_t::field_t const& x, std::string_view const name, F&& f)
{
    if (auto const tmp = details::check_name<P>(x.var, name); not tmp)
        return failure("inside field name: ", tmp.message());
    if (auto const tmp = std::forward<F>(f)(x.type); not tmp)
        return failure("inside field type: ", tmp.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
auto struct_field(std::string_view const name, F&& f)
{
    return [name, f=std::forward<F>(f)] (typename ast::type_def_t<P>::struct_t::field_t const& x)
    {
        return is_struct_field<P>(x, name, f);
    };
}

} // namespace dep0::testing
