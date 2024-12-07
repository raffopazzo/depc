/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Family of functions that compute the wrap-around division of two signed/unsigned integer values.
 *
 * @warning @anchor cpp_int_div_warning
 * It is undefined behaviour to pass input values that violate the following assumptions:
 *   - the two input values must fit inside the appropriate signed/unsigned integer of the given bit width
 *   - the divisor must not be `0`
 *   - for signed division, the result of the division must also fit inside the given bit width;
 *     for example `-128 / -1` does not fit inside `i8_t`.
 */
#pragma once

#include "dep0/ast/ast.hpp"

#include <boost/multiprecision/cpp_int.hpp>

namespace dep0::typecheck {

/**
 * @brief Compute the wrap-around division of two signed `NumBits` integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <std::size_t NumBits>
boost::multiprecision::cpp_int
cpp_int_div_signed(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two unsigned `NumBits` integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <std::size_t NumBits>
boost::multiprecision::cpp_int
cpp_int_div_unsigned(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two signed 8 bit integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <>
boost::multiprecision::cpp_int
cpp_int_div_signed<8>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two signed 16 bit integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <>
boost::multiprecision::cpp_int
cpp_int_div_signed<16>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two signed 32 bit integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <>
boost::multiprecision::cpp_int
cpp_int_div_signed<32>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two signed 64 bit integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <>
boost::multiprecision::cpp_int
cpp_int_div_signed<64>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two unsigned 8 bit integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <>
boost::multiprecision::cpp_int
cpp_int_div_unsigned<8>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two unsigned 16 bit integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <>
boost::multiprecision::cpp_int
cpp_int_div_unsigned<16>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two unsigned 32 bit integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <>
boost::multiprecision::cpp_int
cpp_int_div_unsigned<32>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two unsigned 64 bit integers.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
template <>
boost::multiprecision::cpp_int
cpp_int_div_unsigned<64>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two signed integer values of the given bit width.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
boost::multiprecision::cpp_int
cpp_int_div_signed(
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two unsigned integer values of the given bit width.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
boost::multiprecision::cpp_int
cpp_int_div_unsigned(
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * @brief Compute the wrap-around division of two integer values of the given bit width and sign.
 * @warning @ref cpp_int_div_warning "It is undefined behaviour to violate the assumptions described here".
 */
boost::multiprecision::cpp_int
cpp_int_div(
    ast::sign_t,
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

} // namespace dep0::typecheck
