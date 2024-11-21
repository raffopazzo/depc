/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/cpp_int_mult.hpp"

namespace dep0::typecheck {

template <>
boost::multiprecision::cpp_int
cpp_int_mult_signed<8>(
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    auto const x = std::bit_cast<std::uint8_t>(a.template convert_to<std::int8_t>());
    auto const y = std::bit_cast<std::uint8_t>(b.template convert_to<std::int8_t>());
    std::uint8_t const z = x * y;
    return std::bit_cast<std::int8_t>(z);
}

template <>
boost::multiprecision::cpp_int
cpp_int_mult_signed<16>(
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    auto const x = std::bit_cast<std::uint16_t>(a.template convert_to<std::int16_t>());
    auto const y = std::bit_cast<std::uint16_t>(b.template convert_to<std::int16_t>());
    std::uint16_t const z = x * y;
    return std::bit_cast<std::int16_t>(z);
}

template <>
boost::multiprecision::cpp_int
cpp_int_mult_signed<32>(
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    auto const x = std::bit_cast<std::uint32_t>(a.template convert_to<std::int32_t>());
    auto const y = std::bit_cast<std::uint32_t>(b.template convert_to<std::int32_t>());
    std::uint32_t const z = x * y;
    return std::bit_cast<std::int32_t>(z);
}

template <>
boost::multiprecision::cpp_int
cpp_int_mult_signed<64>(
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    auto const x = std::bit_cast<std::uint64_t>(a.template convert_to<std::int64_t>());
    auto const y = std::bit_cast<std::uint64_t>(b.template convert_to<std::int64_t>());
    std::uint64_t const z = x * y;
    return std::bit_cast<std::int64_t>(z);
}

template <>
boost::multiprecision::cpp_int
cpp_int_mult_unsigned<8>(
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    std::uint8_t const x = a.template convert_to<std::uint8_t>();
    std::uint8_t const y = b.template convert_to<std::uint8_t>();
    std::uint8_t const z = x * y;
    return z;
}

template <>
boost::multiprecision::cpp_int
cpp_int_mult_unsigned<16>(
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    std::uint16_t const x = a.template convert_to<std::uint16_t>();
    std::uint16_t const y = b.template convert_to<std::uint16_t>();
    std::uint16_t const z = x * y;
    return z;
}

template <>
boost::multiprecision::cpp_int
cpp_int_mult_unsigned<32>(
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    std::uint32_t const x = a.template convert_to<std::uint32_t>();
    std::uint32_t const y = b.template convert_to<std::uint32_t>();
    std::uint32_t const z = x * y;
    return z;
}

template <>
boost::multiprecision::cpp_int
cpp_int_mult_unsigned<64>(
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    std::uint64_t const x = a.template convert_to<std::uint64_t>();
    std::uint64_t const y = b.template convert_to<std::uint64_t>();
    std::uint64_t const z = x * y;
    return z;
}

boost::multiprecision::cpp_int
cpp_int_mult_signed(
    ast::width_t const width,
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    switch (width)
    {
    case ast::width_t::_8: return cpp_int_mult_signed<8>(a, b);
    case ast::width_t::_16: return cpp_int_mult_signed<16>(a, b);
    case ast::width_t::_32: return cpp_int_mult_signed<32>(a, b);
    case ast::width_t::_64: return cpp_int_mult_signed<64>(a, b);
    default: __builtin_unreachable();
    }
}

boost::multiprecision::cpp_int
cpp_int_mult_unsigned(
    ast::width_t const width,
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    switch (width)
    {
    case ast::width_t::_8: return cpp_int_mult_unsigned<8>(a, b);
    case ast::width_t::_16: return cpp_int_mult_unsigned<16>(a, b);
    case ast::width_t::_32: return cpp_int_mult_unsigned<32>(a, b);
    case ast::width_t::_64: return cpp_int_mult_unsigned<64>(a, b);
    default: __builtin_unreachable();
    }
}

boost::multiprecision::cpp_int
cpp_int_mult(
    ast::sign_t const sign,
    ast::width_t const width,
    boost::multiprecision::cpp_int const& a,
    boost::multiprecision::cpp_int const& b)
{
    return sign == ast::sign_t::signed_v ? cpp_int_mult_signed(width, a, b) : cpp_int_mult_unsigned(width, a, b);
}

} // namespace dep0::typecheck
