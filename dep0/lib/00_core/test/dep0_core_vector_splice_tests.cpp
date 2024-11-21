/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_core_vector_splice_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/vector_splice.hpp"

#include <concepts>
#include <ostream>

namespace dep0 {

class move_only_integer
{
    int value;

public:
    /*implicit*/move_only_integer(int v) : value(v) {}
    move_only_integer(move_only_integer const&) = delete;
    move_only_integer& operator=(move_only_integer const&) = delete;
    move_only_integer(move_only_integer&&) = default;
    move_only_integer& operator=(move_only_integer&&) = default;

    bool operator==(move_only_integer const&) const = default;

    friend std::ostream& operator<<(std::ostream& os, move_only_integer const&);
};

std::ostream& operator<<(std::ostream& os, move_only_integer const& x)
{
    return os << x.value;
}

template <std::integral... T>
std::vector<move_only_integer> vector_of(T... values)
{
    std::vector<move_only_integer> v;
    v.reserve(sizeof...(values));
    (v.push_back(values), ...);
    return v;
}

BOOST_AUTO_TEST_SUITE(dep0_core_vector_splice_tests)

BOOST_AUTO_TEST_CASE(erase_begin)
{
    auto v = vector_of(1,2,3,4,5);
    auto const it = vector_splice(v, v.begin(), vector_of());
    BOOST_CHECK(it == v.begin());
    BOOST_TEST(v == vector_of(2,3,4,5), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(erase_mid)
{
    auto v = vector_of(1,2,3,4,5);
    auto const it = vector_splice(v, std::next(v.begin(), 3), vector_of());
    BOOST_REQUIRE(it == std::next(v.begin(), 3));
    BOOST_TEST(*it == 5);
    BOOST_TEST(v == vector_of(1,2,3,5), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(erase_end) // no-op
{
    auto v = vector_of(1,2,3,4,5);
    auto const it = vector_splice(v, v.end(), vector_of());
    BOOST_CHECK(it == v.end());
    BOOST_TEST(v == vector_of(1,2,3,4,5), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(splice_begin)
{
    auto v = vector_of(1,2,3,4,5);
    auto const it = vector_splice(v, v.begin(), vector_of(6,7));
    BOOST_REQUIRE(it == v.begin());
    BOOST_TEST(*it == 6);
    BOOST_TEST(v == vector_of(6,7,2,3,4,5), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(splice_mid)
{
    auto v = vector_of(1,2,3,4,5);
    auto const it = vector_splice(v, std::next(v.begin(), 3), vector_of(6,7));
    BOOST_REQUIRE(it == std::next(v.begin(), 3));
    BOOST_TEST(*it == 6);
    BOOST_TEST(v == vector_of(1,2,3,6,7,5), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(splice_end)
{
    auto v = vector_of(1,2,3,4,5);
    auto const it = vector_splice(v, v.end(), vector_of(6,7));
    BOOST_REQUIRE(it != v.end());
    BOOST_TEST(*it == 6);
    BOOST_TEST(v == vector_of(1,2,3,4,5,6,7), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0
