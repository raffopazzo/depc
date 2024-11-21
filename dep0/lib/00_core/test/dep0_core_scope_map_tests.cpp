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
    BOOST_TEST_REQUIRE(inserted);
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
    auto map2 = map1;
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
    BOOST_TEST_REQUIRE(inserted2);
    BOOST_TEST_REQUIRE(inserted1);
    BOOST_TEST(it2->second == 99);
    BOOST_TEST(it1->second == 23); // parent not affected by shadowing

    // if child inserts first, parent doesn't see it
    auto [it3, inserted3] = map2.try_emplace("foo", 72);
    BOOST_TEST_REQUIRE(inserted3);
    BOOST_TEST(not map1["foo"]);

    // if parent inserts with same name, child still sees its own one
    auto [it4, inserted4] = map1.try_emplace("foo", 49);
    BOOST_TEST_REQUIRE(inserted4);
    BOOST_TEST(it4->second == 49);
    BOOST_TEST(it3->second == 72);
    BOOST_TEST(map1["foo"]);

    // if parent inserts new, child can see it
    auto [it5, inserted5] = map1.try_emplace("bar", 55);
    BOOST_TEST_REQUIRE(inserted5);
    BOOST_TEST(it5->second == 55);
    auto p1 = map2["bar"];
    BOOST_TEST_REQUIRE(p1);
    BOOST_TEST(*p1 == 55);

    // but child can still shadow it
    auto [it6, inserted6] = map2.try_emplace("bar", 33);
    BOOST_TEST_REQUIRE(inserted6);
    BOOST_TEST(it6->second == 33);
    BOOST_TEST(*p1 == 55); // obviously the old reference does not refer to the new value, you need a new reference
    auto p2 = map2["bar"];
    BOOST_TEST_REQUIRE(p2);
    BOOST_TEST(*p2 == 33);
}

BOOST_AUTO_TEST_CASE(parent_tests)
{
    scope_map<std::string, int> map1;
    map1.try_emplace("bar", 33);
    BOOST_TEST(not map1.parent().has_value());
    auto map2 = map1.extend();
    auto parent = map2.parent();
    BOOST_TEST_REQUIRE(parent.has_value());
    auto [it1, inserted1] = map2.try_emplace("foo", 77);
    BOOST_TEST(inserted1);
    BOOST_TEST(it1->second == 77);
    BOOST_TEST(not (*parent)["foo"]);
    BOOST_TEST(not map1["foo"]);
    auto [it2, inserted2] = map2.try_emplace("bar", 99);
    BOOST_TEST(inserted2);
    BOOST_TEST(it2->second == 99);
    auto* p1 = (*parent)["bar"];
    auto* p2 = map1["bar"];
    BOOST_TEST_REQUIRE(p1);
    BOOST_TEST_REQUIRE(p2);
    BOOST_TEST(*p1 == 33);
    BOOST_TEST(*p2 == 33);
    *p1 = 55;
    BOOST_TEST(*p2 == 55);
    BOOST_TEST(*(*parent)["bar"] == 55);
    BOOST_TEST(*(map1["bar"]) == 55);
    auto [it3, inserted3] = parent->try_emplace("baz", 42);
    BOOST_TEST(inserted3);
    BOOST_TEST(it3->second == 42);
    BOOST_TEST_REQUIRE(map1["baz"]);
    BOOST_TEST(*map1["baz"] == 42);
    *map1["baz"] = 43;
    BOOST_TEST(it3->second == 43);
    auto [_, inserted_again] = map1.try_emplace("baz", 42);
    BOOST_TEST(not inserted_again);

    BOOST_TEST_REQUIRE(map2["baz"]); // through parent
    BOOST_TEST(*(map2["baz"]) == 43);
    auto [it4, inserted4] = map2.try_emplace("baz", 42);
    BOOST_TEST(inserted4);
    BOOST_TEST(it4->second == 42);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0
