/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#define BOOST_TEST_MODULE dep0_core_scope_map_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/scope_map.hpp"

#include <string>
#include <type_traits>

namespace dep0 {

BOOST_AUTO_TEST_SUITE(dep0_core_scope_map_tests)

BOOST_AUTO_TEST_CASE(single_level_tests)
{
    scope_map<std::string, int> map;
    BOOST_TEST(not map["hello"]);
    auto [it, inserted] = map.try_emplace("hello", 23);
    BOOST_TEST(inserted);
    BOOST_TEST(it->first == "hello");
    BOOST_TEST(it->second == 23);
    auto el = map["hello"];
    BOOST_TEST_REQUIRE(el);
    BOOST_TEST(*el == 23);
    it->second = 99;
    BOOST_TEST(it->second == 99);
    BOOST_TEST(*el == 99);

    auto const [_, inserted_again] = map.try_emplace("hello", 23);
    BOOST_TEST(not inserted_again);
}

BOOST_AUTO_TEST_CASE(copy_tests)
{
    scope_map<std::string, int> map1;
    auto [it1, inserted1] = map1.try_emplace("hello", 23);
    // A copy behaves like the original, so adding a duplicate will fail
    auto map2 = map1.copy();
    auto [it2, inserted2] = map2.try_emplace("hello", 99);
    BOOST_TEST(inserted1);
    BOOST_TEST(not inserted2);
    BOOST_TEST(it1->second == 23);
    BOOST_TEST(it2->second == 23);

    // But adding to the copy does not alter the original
    auto [it3, inserted3] = map2.try_emplace("foo", 99);
    BOOST_TEST(inserted3);
    BOOST_TEST(it3->second == 99);
    BOOST_TEST(not map1["foo"]);

    // Similarly adding to the original does not alter the copy
    auto [it4, inserted4] = map1.try_emplace("foo", 77);
    BOOST_TEST(inserted4);
    BOOST_TEST(it4->second == 77);
    BOOST_TEST(it3->second == 99);
}

BOOST_AUTO_TEST_CASE(extend_tests)
{
    scope_map<std::string, int> map1;
    auto [it1, inserted1] = map1.try_emplace("hello", 23);

    auto map2 = map1.extend();
    // extending is different from copying, because in the new level you can add a shadowing element
    auto [it2, inserted2] = map2.try_emplace("hello", 99);
    BOOST_TEST(inserted2);
    BOOST_TEST(inserted1);
    BOOST_TEST(it2->second == 99);
    BOOST_TEST(it1->second == 23); // parent not affected by shadowing

    // if child inserts a new element, parent doesn't see it
    auto [it3, inserted3] = map2.try_emplace("foo", 72);
    BOOST_TEST(inserted3);
    BOOST_TEST(not map1["foo"]);

    // if parent inserts with same name, child still sees its own one
    auto [it4, inserted4] = map1.try_emplace("foo", 49);
    BOOST_TEST(inserted4);
    BOOST_TEST(it4->second == 49);
    BOOST_TEST(it3->second == 72);
    BOOST_TEST(map1["foo"]);

    // if parent inserts new, child does not see it
    auto [it5, inserted5] = map1.try_emplace("bar", 55);
    BOOST_TEST(inserted5);
    BOOST_TEST(it5->second == 55);
    BOOST_TEST(not map2["bar"]);
    auto const p1 = map1["bar"];
    BOOST_TEST_REQUIRE(p1);
    BOOST_TEST(*p1 == 55);

    // and child can still shadow it
    auto [it6, inserted6] = map2.try_emplace("bar", 33);
    BOOST_TEST(inserted6);
    BOOST_TEST(it6->second == 33);
    BOOST_TEST(*p1 == 55); // obviously the parent reference does not refer to the child value
    auto const p2 = map2["bar"];
    BOOST_TEST_REQUIRE(p2);
    BOOST_TEST(*p2 == 33);
}

BOOST_AUTO_TEST_CASE(iterate_tests)
{
    scope_map<std::string, int> map1;
    map1.try_emplace("bar", 33).first;
    auto map2 = map1.extend();
    auto map3 = map2.extend();
    map3.try_emplace("foo", 99);
    map3.try_emplace("bar", 42);
    map1.try_emplace("baz", 77);

    auto entries = std::vector<std::pair<std::string, int>>(map1.cbegin(), map1.cend());
    BOOST_TEST_REQUIRE(entries.size() == 2ul);
    BOOST_TEST(entries[0].first == "bar");
    BOOST_TEST(entries[0].second == 33);
    BOOST_TEST(entries[1].first == "baz");
    BOOST_TEST(entries[1].second == 77);
    // and reverse
    entries.assign(map1.rbegin(), map1.rend());
    BOOST_TEST_REQUIRE(entries.size() == 2ul);
    BOOST_TEST(entries[0].first == "baz");
    BOOST_TEST(entries[0].second == 77);
    BOOST_TEST(entries[1].first == "bar");
    BOOST_TEST(entries[1].second == 33);
    // innermost only
    entries.assign(map1.innermost_begin(), map1.innermost_end());
    BOOST_TEST_REQUIRE(entries.size() == 2ul);
    BOOST_TEST(entries[0].first == "bar");
    BOOST_TEST(entries[0].second == 33);
    BOOST_TEST(entries[1].first == "baz");
    BOOST_TEST(entries[1].second == 77);
    // again but with range
    entries.clear();
    std::ranges::copy(map1.innermost_range(), std::back_inserter(entries));
    BOOST_TEST_REQUIRE(entries.size() == 2ul);
    BOOST_TEST(entries[0].first == "bar");
    BOOST_TEST(entries[0].second == 33);
    BOOST_TEST(entries[1].first == "baz");
    BOOST_TEST(entries[1].second == 77);

    entries.assign(map2.cbegin(), map2.cend());
    BOOST_TEST_REQUIRE(entries.size() == 1ul);
    BOOST_TEST(entries[0].first == "bar");
    BOOST_TEST(entries[0].second == 33);
    // and reverse
    entries.assign(map2.rbegin(), map2.rend());
    BOOST_TEST_REQUIRE(entries.size() == 1ul);
    BOOST_TEST(entries[0].first == "bar");
    BOOST_TEST(entries[0].second == 33);
    // innermost only
    entries.assign(map2.innermost_begin(), map2.innermost_end());
    BOOST_TEST(entries.size() == 0ul);
    // again but with range
    entries.clear();
    std::ranges::copy(map2.innermost_range(), std::back_inserter(entries));
    BOOST_TEST(entries.size() == 0ul);

    entries.assign(map3.cbegin(), map3.cend());
    BOOST_TEST_REQUIRE(entries.size() == 3ul);
    BOOST_TEST(entries[0].first == "bar");
    BOOST_TEST(entries[0].second == 33);
    BOOST_TEST(entries[1].first == "foo");
    BOOST_TEST(entries[1].second == 99);
    BOOST_TEST(entries[2].first == "bar");
    BOOST_TEST(entries[2].second == 42);
    // and reverse
    entries.assign(map3.rbegin(), map3.rend());
    BOOST_TEST_REQUIRE(entries.size() == 3ul);
    BOOST_TEST(entries[0].first == "bar");
    BOOST_TEST(entries[0].second == 42);
    BOOST_TEST(entries[1].first == "foo");
    BOOST_TEST(entries[1].second == 99);
    BOOST_TEST(entries[2].first == "bar");
    BOOST_TEST(entries[2].second == 33);
    // innermost only
    entries.assign(map3.innermost_begin(), map3.innermost_end());
    BOOST_TEST_REQUIRE(entries.size() == 2ul);
    BOOST_TEST(entries[0].first == "foo");
    BOOST_TEST(entries[0].second == 99);
    BOOST_TEST(entries[1].first == "bar");
    BOOST_TEST(entries[1].second == 42);
    // again but with range
    entries.clear();
    std::ranges::copy(map3.innermost_range(), std::back_inserter(entries));
    BOOST_TEST_REQUIRE(entries.size() == 2ul);
    BOOST_TEST(entries[0].first == "foo");
    BOOST_TEST(entries[0].second == 99);
    BOOST_TEST(entries[1].first == "bar");
    BOOST_TEST(entries[1].second == 42);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0
