#pragma once

#include "dep0/ast/ast.hpp"

#include <boost/multiprecision/cpp_int.hpp>

namespace dep0::typecheck {

/**
 * Family of functions that compute the signed/unsigned wrap-around division of two integer values.
 * All functions assume that the two integer values fit inside `NumBits`.
 * @{
 */
template <std::size_t NumBits>
boost::multiprecision::cpp_int
cpp_int_div_signed(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <std::size_t NumBits>
boost::multiprecision::cpp_int
cpp_int_div_unsigned(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_div_signed<8>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_div_signed<16>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_div_signed<32>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_div_signed<64>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_div_unsigned<8>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_div_unsigned<16>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_div_unsigned<32>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_div_unsigned<64>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);
/** @} */

/**
 * Compute the signed wrap-around division of two integer values.
 * This function assumes that the two integer values fit inside the given bit width.
 */
boost::multiprecision::cpp_int
cpp_int_div_signed(
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the unsigned wrap-around division of two integer values.
 * This function assumes that the two integer values fit inside the given bit width.
 */
boost::multiprecision::cpp_int
cpp_int_div_unsigned(
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the signed/unsigned wrap-around division of two integer values.
 * This function assumes that the two integer values fit inside the given bit width.
 */
boost::multiprecision::cpp_int
cpp_int_div(
    ast::sign_t,
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

} // namespace dep0::typecheck
