#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include <string_view>

namespace dep0::parser {

/**
 * Allocation-free parsing of `boost::multiprecision::cpp_int`
 * from a `std::string_view`.
 *
 * @remarks What we mean by 'allocation-free' is that this
 *          function will read characters from the string_view,
 *          as opposed to allocating from an istringstream.
 *          Allocation may still occur inside cpp_int if the number
 *          is too large. Also, if the input string contains a
 *          digit separator, allocation might still be required.
 */
boost::multiprecision::cpp_int parse_cpp_int(std::string_view);

} // namespace dep0::parser
