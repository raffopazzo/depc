/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Family of functions to query the minimum/maximum value of signed/unsigned integer types.
 */
#pragma once

#include "dep0/ast/ast.hpp"

#include <boost/multiprecision/cpp_int.hpp>

namespace dep0::typecheck {

/** @brief Returns the minimum signed integer value that fits inside the given number of bits. */
template <std::size_t NumBits> boost::multiprecision::cpp_int cpp_int_min_signed();

/** @brief Returns the maximum signed integer value that fits inside the given number of bits. */
template <std::size_t NumBits> boost::multiprecision::cpp_int cpp_int_max_signed();

/** @brief Returns the maximum unsigned integer value that fits inside the given number of bits. */
template <std::size_t NumBits> boost::multiprecision::cpp_int cpp_int_max_unsigned();

/** @brief Returns the minimum value for a signed 8-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_min_signed<8>();

/** @brief Returns the minimum value for a signed 16-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_min_signed<16>();

/** @brief Returns the minimum value for a signed 32-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_min_signed<32>();

/** @brief Returns the minimum value for a signed 64-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_min_signed<64>();

/** @brief Returns the maximum value for a signed 8-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_max_signed<8>();

/** @brief Returns the maximum value for a signed 16-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_max_signed<16>();

/** @brief Returns the maximum value for a signed 32-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_max_signed<32>();

/** @brief Returns the maximum value for a signed 64-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_max_signed<64>();

/** @brief Returns the maximum value for an unsigned 8-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<8>();

/** @brief Returns the maximum value for an unsigned 16-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<16>();

/** @brief Returns the maximum value for an unsigned 32-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<32>();

/** @brief Returns the maximum value for an unsigned 64-bit integer. */
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<64>();

/** @brief Returns the minimum signed integer value that fits inside the given bit width. */
boost::multiprecision::cpp_int cpp_int_min_signed(ast::width_t);

/** @brief Returns the maximum signed integer value that fits inside the given bit width. */
boost::multiprecision::cpp_int cpp_int_max_signed(ast::width_t);

/** @brief Returns the maximum unsigned integer value that fits inside the given bit width. */
boost::multiprecision::cpp_int cpp_int_max_unsigned(ast::width_t);

/**
 * @brief Returns the minimum signed/unsigned integer value that fits inside the given bit width.
 *
 * For unsigned integers, this will be 0.
 */
boost::multiprecision::cpp_int cpp_int_min(ast::sign_t, ast::width_t);

/** @brief Returns the maximum signed/unsigned integer value that fits inside the given bit width. */
boost::multiprecision::cpp_int cpp_int_max(ast::sign_t, ast::width_t);

} // namespace dep0::typecheck
