#pragma once

#include "dep0/ast/ast.hpp"

#include <boost/multiprecision/cpp_int.hpp>

namespace dep0::typecheck {

template <std::size_t NumBits>
boost::multiprecision::cpp_int
cpp_int_add_signed(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <std::size_t NumBits>
boost::multiprecision::cpp_int
cpp_int_add_unsigned(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_add_signed<8>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_add_signed<16>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_add_signed<32>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_add_signed<64>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_add_unsigned<8>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_add_unsigned<16>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_add_unsigned<32>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

template <>
boost::multiprecision::cpp_int
cpp_int_add_unsigned<64>(
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

boost::multiprecision::cpp_int
cpp_int_add_signed(
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

boost::multiprecision::cpp_int
cpp_int_add_unsigned(
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

boost::multiprecision::cpp_int
cpp_int_add(
    ast::sign_t,
    ast::width_t,
    boost::multiprecision::cpp_int const&,
    boost::multiprecision::cpp_int const&);

} // namespace dep0::typecheck

