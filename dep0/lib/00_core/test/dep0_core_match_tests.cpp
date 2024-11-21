/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_core_match_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/match.hpp"

#include <string>

struct dummy_t
{
    int value;
};

std::string to_str(int const x) { return std::to_string(x); }
std::string to_str(dummy_t const x) { return std::to_string(x.value); }

BOOST_AUTO_TEST_SUITE(dep0_core_match_tests)

BOOST_AUTO_TEST_CASE(one_match_single)
{
    std::variant<int> x{23};
    auto const f = [] (int const& x) { return to_str(x); };
    BOOST_TEST(dep0::match(x, f) == "23");
}

BOOST_AUTO_TEST_CASE(one_match_two)
{
    std::variant<int, dummy_t> x{23}, y{dummy_t{2ul}};
    auto const f = [] (auto const& x) { return to_str(x); };
    BOOST_TEST(dep0::match(x, f) == "23");
    BOOST_TEST(dep0::match(y, f) == "2");
}

BOOST_AUTO_TEST_CASE(two_match_three)
{
    std::variant<int, dummy_t, std::string> x{23}, y{dummy_t{2ul}}, z{"test"};
    auto const f1 = [] (std::string const& x) { return x; };
    auto const f2 = [] (auto const& x) { return to_str(x); };
    BOOST_TEST(dep0::match(x, f1, f2) == "23");
    BOOST_TEST(dep0::match(y, f1, f2) == "2");
    BOOST_TEST(dep0::match(z, f1, f2) == "test");
}

BOOST_AUTO_TEST_CASE(three_match_three_out_of_order)
{
    std::variant<int, dummy_t, std::string> x{23}, y{dummy_t{2ul}}, z{"test"};
    auto const f1 = [] (int const& x) { return to_str(x); };
    auto const f2 = [] (dummy_t const& x) { return to_str(x); };
    auto const f3 = [] (std::string const& x) { return x; };
    BOOST_TEST(dep0::match(x, f3, f1, f2) == "23");
    BOOST_TEST(dep0::match(y, f3, f1, f2) == "2");
    BOOST_TEST(dep0::match(z, f3, f1, f2) == "test");
}

BOOST_AUTO_TEST_CASE(return_const_ref)
{
    std::variant<int, dummy_t> x{dummy_t{23}};
    auto const& y = x;
    int const& ref =
        dep0::match(
            y,
            [] (int const& i) -> int const& { return i; },
            [] (dummy_t const& w) -> int const& { return w.value; });
    BOOST_TEST(ref == 23);
    std::get<dummy_t>(x).value = 99;
    BOOST_TEST(ref == 99);
}

BOOST_AUTO_TEST_CASE(return_ref)
{
    std::variant<int, dummy_t> x{dummy_t{23}};
    int& ref =
        dep0::match(
            x,
            [] (int& i) -> int& { return i; },
            [] (dummy_t& w) -> int& { return w.value; });
    ref = 99;
    BOOST_TEST(std::get<dummy_t>(x).value == 99);
}

BOOST_AUTO_TEST_SUITE_END()
