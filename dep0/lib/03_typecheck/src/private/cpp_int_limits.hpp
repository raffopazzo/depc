#pragma once

#include "dep0/ast/ast.hpp"

#include <boost/multiprecision/cpp_int.hpp>

namespace dep0::typecheck {

template <std::size_t NumBits> boost::multiprecision::cpp_int cpp_int_min_signed();
template <std::size_t NumBits> boost::multiprecision::cpp_int cpp_int_max_signed();
template <std::size_t NumBits> boost::multiprecision::cpp_int cpp_int_max_unsigned();

template <> boost::multiprecision::cpp_int cpp_int_min_signed<8>();
template <> boost::multiprecision::cpp_int cpp_int_min_signed<16>();
template <> boost::multiprecision::cpp_int cpp_int_min_signed<32>();
template <> boost::multiprecision::cpp_int cpp_int_min_signed<64>();

template <> boost::multiprecision::cpp_int cpp_int_max_signed<8>();
template <> boost::multiprecision::cpp_int cpp_int_max_signed<16>();
template <> boost::multiprecision::cpp_int cpp_int_max_signed<32>();
template <> boost::multiprecision::cpp_int cpp_int_max_signed<64>();

template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<8>();
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<16>();
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<32>();
template <> boost::multiprecision::cpp_int cpp_int_max_unsigned<64>();

boost::multiprecision::cpp_int cpp_int_min_signed(ast::width_t);
boost::multiprecision::cpp_int cpp_int_max_signed(ast::width_t);
boost::multiprecision::cpp_int cpp_int_max_unsigned(ast::width_t);

boost::multiprecision::cpp_int cpp_int_min(ast::sign_t, ast::width_t);
boost::multiprecision::cpp_int cpp_int_max(ast::sign_t, ast::width_t);

} // namespace dep0::typecheck
