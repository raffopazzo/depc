#pragma once

#include "dep0/ast/ast.hpp"

#include <boost/multiprecision/cpp_int.hpp>

namespace dep0::typecheck {

/**
 * Compute the signed-integer-wrapping-addition of the input vaules with bit width of `NumBits`.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside `NumBits`.
 */
template <std::size_t NumBits>
boost::multiprecision::cpp_int
cpp_int_add_signed(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the unsigned-integer-wrapping-addition of the input vaules with bit width of `NumBits`.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside `NumBits`.
 */
template <std::size_t NumBits>
boost::multiprecision::cpp_int
cpp_int_add_unsigned(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the 8-bit signed-integer-wrapping-addition of the input vaules.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside 8-bit.
 */
template <>
boost::multiprecision::cpp_int
cpp_int_add_signed<8>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the 16-bit signed-integer-wrapping-addition of the input vaules.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside 16-bit.
 */
template <>
boost::multiprecision::cpp_int
cpp_int_add_signed<16>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the 32-bit signed-integer-wrapping-addition of the input vaules.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside 32-bit.
 */
template <>
boost::multiprecision::cpp_int
cpp_int_add_signed<32>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the 64-bit signed-integer-wrapping-addition of the input vaules.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside 64-bit.
 */
template <>
boost::multiprecision::cpp_int
cpp_int_add_signed<64>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the 8-bit unsigned-integer-wrapping-addition of the input vaules.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside 8-bit.
 */
template <>
boost::multiprecision::cpp_int
cpp_int_add_unsigned<8>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the 16-bit unsigned-integer-wrapping-addition of the input vaules.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside 16-bit.
 */
template <>
boost::multiprecision::cpp_int
cpp_int_add_unsigned<16>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the 32-bit unsigned-integer-wrapping-addition of the input vaules.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside 32-bit.
 */
template <>
boost::multiprecision::cpp_int
cpp_int_add_unsigned<32>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the 64-bit unsigned-integer-wrapping-addition of the input vaules.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside 64-bit.
 */
template <>
boost::multiprecision::cpp_int
cpp_int_add_unsigned<64>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the signed-integer-wrapping-addition of the input vaules with the specified bit width.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside the specified bit width.
 */
boost::multiprecision::cpp_int
cpp_int_add_signed(
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the unsigned-integer-wrapping-addition of the input vaules with the specified bit width.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside the specified bit width.
 */
boost::multiprecision::cpp_int
cpp_int_add_unsigned(
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

/**
 * Compute the (signed/unsigned)-integer-wrapping-addition of the input vaules with the specified bit width.
 *
 * Because this function is part of the typechecking stage,
 * both input values will be assumed to have an inital value that fits inside the specified bit width.
 */
boost::multiprecision::cpp_int
cpp_int_add(
    ast::sign_t,
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

} // namespace dep0::typecheck

