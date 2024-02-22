#include "private/cpp_int_limits.hpp"

#include <limits>

namespace dep0::typecheck {

template <> boost::multiprecision::cpp_int cpp_int_min_signed<8>()
{
    return std::numeric_limits<std::int8_t>::min();
}
template <> boost::multiprecision::cpp_int cpp_int_min_signed<16>()
{
    return std::numeric_limits<std::int16_t>::min();
}
template <> boost::multiprecision::cpp_int cpp_int_min_signed<32>()
{
    return std::numeric_limits<std::int32_t>::min();
}
template <> boost::multiprecision::cpp_int cpp_int_min_signed<64>() 
{
    return std::numeric_limits<std::int64_t>::min();
}

template <> boost::multiprecision::cpp_int cpp_int_max_signed<8>()
{
    return std::numeric_limits<std::int8_t>::max();
}
template <> boost::multiprecision::cpp_int cpp_int_max_signed<16>()
{
    return std::numeric_limits<std::int16_t>::max();
}
template <> boost::multiprecision::cpp_int cpp_int_max_signed<32>()
{
    return std::numeric_limits<std::int32_t>::max();
}
template <> boost::multiprecision::cpp_int cpp_int_max_signed<64>()
{
    return std::numeric_limits<std::int64_t>::max();
}

template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<8>()
{
    return std::numeric_limits<std::uint8_t>::max();
}
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<16>()
{
    return std::numeric_limits<std::uint16_t>::max();
}
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<32>()
{
    return std::numeric_limits<std::uint32_t>::max();
}
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<64>()
{
    return std::numeric_limits<std::uint64_t>::max();
}

boost::multiprecision::cpp_int cpp_int_min_signed(ast::width_t const w)
{
    switch (w)
    {
        using enum ast::width_t;
        case _8:  return cpp_int_min_signed<8>();
        case _16: return cpp_int_min_signed<16>();
        case _32: return cpp_int_min_signed<32>();
        default:  return cpp_int_min_signed<64>();
    }
}

boost::multiprecision::cpp_int cpp_int_max_signed(ast::width_t const w)
{
    switch (w)
    {
        using enum ast::width_t;
        case _8:  return cpp_int_max_signed<8>();
        case _16: return cpp_int_max_signed<16>();
        case _32: return cpp_int_max_signed<32>();
        default:  return cpp_int_max_signed<64>();
    }
}

boost::multiprecision::cpp_int cpp_int_max_unsigned(ast::width_t const w)
{
    switch (w)
    {
        using enum ast::width_t;
        case _8:  return cpp_int_max_unsigned<8>();
        case _16: return cpp_int_max_unsigned<16>();
        case _32: return cpp_int_max_unsigned<32>();
        default:  return cpp_int_max_unsigned<64>();
    }
}

boost::multiprecision::cpp_int cpp_int_min(ast::sign_t const sign, ast::width_t const w)
{
    return sign == ast::sign_t::signed_v ? cpp_int_min_signed(w) : 0;
}

boost::multiprecision::cpp_int cpp_int_max(ast::sign_t const sign, ast::width_t const w)
{
    return sign == ast::sign_t::signed_v ? cpp_int_max_signed(w) : cpp_int_max_unsigned(w);
}

} // namespace dep0::typecheck
