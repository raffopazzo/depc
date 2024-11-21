/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

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

inline constexpr auto is_cstr =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::cstr_t>(type.value))
        return true;
    else
        return failure("type is not cstr_t but ", pretty_name(type.value));
};

inline constexpr auto is_i8 =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::i8_t>(type.value))
        return true;
    else
        return failure("type is not i8_t but ", pretty_name(type.value));
};

inline constexpr auto is_i16 =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::i16_t>(type.value))
        return true;
    else
        return failure("type is not i16_t but ", pretty_name(type.value));
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

inline constexpr auto is_i64 =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::i64_t>(type.value))
        return true;
    else
        return failure("type is not i64_t but ", pretty_name(type.value));
};

inline constexpr auto is_u8 =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::u8_t>(type.value))
        return true;
    else
        return failure("type is not u8_t but ", pretty_name(type.value));
};

inline constexpr auto is_u16 =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::u16_t>(type.value))
        return true;
    else
        return failure("type is not u16_t but ", pretty_name(type.value));
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

inline constexpr auto is_u64 =
[] <ast::Properties P> (ast::expr_t<P> const& type)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::u64_t>(type.value))
        return true;
    else
        return failure("type is not u64_t but ", pretty_name(type.value));
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

} // namespace dep0::testing
