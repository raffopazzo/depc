/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

/**
 * @file
 * @brief A family of functions to print the AST in a fairly human-readable format.
 *
 * Each `pretty_print` function will start writing from the current stream position;
 * if a new line is needed, it will write the new line character and the correct number of indentation spaces.
 * In this way, each call will move the character stream to the "correct" starting position for the next call.
 */

#pragma once

#include "dep0/ast/ast.hpp"

#include <ostream>

namespace dep0::ast {

/**
 * @brief Print the AST to the given output stream with pretty indentation spaces if needed.
 * @return The same output stream for method chaiing, for example `pretty_print(std::cout, x) << std::endl`.
 */
template <Properties P>
std::ostream& pretty_print(std::ostream&, module_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, type_def_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, axiom_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, extern_decl_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, func_decl_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, func_def_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, func_arg_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, body_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, stmt_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename stmt_t<P>::if_else_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename stmt_t<P>::return_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename stmt_t<P>::impossible_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, expr_t<P> const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::typename_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::true_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::auto_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::bool_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::cstr_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::unit_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::i8_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::i16_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::i32_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::i64_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::u8_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::u16_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::u32_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::u64_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::boolean_constant_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::numeric_constant_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::string_literal_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::boolean_expr_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::relation_expr_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::arith_expr_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::var_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::global_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::app_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::abs_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::pi_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::array_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::init_list_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::subscript_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(std::ostream&, typename expr_t<P>::because_t const&, std::size_t indent = 0ul);

template <Properties P>
std::ostream& pretty_print(
    std::ostream&,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    std::size_t indent = 0ul);

} // namespace dep0::ast

#include "dep0/ast/pretty_print_impl.hpp"
