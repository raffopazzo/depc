/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::scope_map`.
 */
#pragma once

#include "dep0/source.hpp"

#include <memory>
#include <map>
#include <optional>

namespace dep0 {

/**
 * @brief Key-Value pair dictionary that can be inherited to allow shadowing inside a different scope.
 *
 * This is useful for example to track variables declared inside a function scope which
 * might shadow other variables with the same name declared in outer function scopes.
 */
template <typename K, typename V>
class scope_map
{
    // TODO could be more clever here, eg using a dense map and maybe a custom comparator for source_text
    // that compares length first and then the 2 strings in reverse, but first we need benchmarks...
    // the important thing is to use a map that doesn't invalidate iterators and pointers/references
    using data_map = std::map<K, V>;

    struct state_t;
    std::shared_ptr<state_t> state;

    explicit scope_map(std::shared_ptr<state_t>);

public:
    using iterator = typename data_map::iterator;
    using const_iterator = typename data_map::const_iterator;

    /** @brief Constructs an empty scope. */
    scope_map();

    /**
     * @brief Constructs a copy that behaves like the original but separate from other copies.
     * 
     * For example, adding a duplicate entry will fail.
     * But if you add a new entry to the copy, the original will not see it.
     *
     * Obviously you can also extend the new copy as normal.
     */
    scope_map(scope_map const&);

    /** @brief Copy-assignment behaves like the copy-constructor. */
    scope_map& operator=(scope_map const&);

    /**
     * @brief Steals the content from another scope.
     *
     * The moved-out object cannot be used; it can only be destroyed.
     */
    scope_map(scope_map&&) = default;

    /**
     * @brief Move-assignment behaves like the move-constructor.
     *
     * In particular, the moved-out object cannot be used; it can only be destroyed.
     */
    scope_map& operator=(scope_map&&) = default;

    /**
     * @brief Const-iterator to the key-value pairs at the current scope level.
     *
     * Key-value pairs of the parent level are not visited.
     * To also iterate on the parent level you have to invoke `parent()` and iterate on its result.
     * Note, however, that in doing so you will also iterate on any key-value pair shadowed by child levels.
     */
    const_iterator begin() const;

    /** @brief End iterator at the current scope level. */
    const_iterator end() const;

    /**
     * @brief Creates a new scope that extends the current one, where new key-value pairs can shadow old ones.
     * 
     * - Adding a new key to the new scope will succeed even if the same key already existed in the parent scope.
     * - However, adding the same key multiple times to the new scope will only succeed once.
     * - A lookup from the new scope will return the new value, whilst from the parent scope will return the old one.
     * - A new key added to the new scope, that does not exist in the old scope, will not appear in the old scope.
     */
    scope_map extend() const;

    /**
     * @brief Return the parent of this scope or `nullopt` if this is the root scope.
     *
     * Changes to the parent scope made via the returned object will be reflected to the original one and viceversa.
     */
    std::optional<scope_map> parent();

    /**
     * @brief Const-propagating overload.
     *
     * Similarly to the non-const overload, changes to the parent scope will be reflected to the returned object.
     */
    std::optional<scope_map const> parent() const;

    /**
     * @brief Inserts a new key-value pair, if the key does not already exist at the current scope.
     * 
     * Insertion succeeds if and only if there is no element with the given key at the current scope level,
     * even though there is one at the parent scope level, thereby shadowing the parent one.
     *
     * In any case, if insertion takes place the value is constructed in-place by forwarding all remaining arguments.
     *
     * @return The iterator to the key-value pair with a flag which is true if insertion took place.
     */
    template <typename... Args> std::pair<iterator, bool> try_emplace(K const&, Args&&...);

    /**
     * @brief Overload taking an rvalue-reference to the key.
     *
     * Except for this, every other detail is the same as the overload taking a const-reference.
     */
    template <typename... Args> std::pair<iterator, bool> try_emplace(K&&, Args&&...);

    // Find an element by the given key, either at the current scope level or any parent level.
    // Return nullptr if the element does not exist at any scope level or a (stable) pointer to the found element.

    /**
     * @brief Find an element by the given key, either at the current scope level or any parent level.
     *
     * Because of shadowing, this may require a lookup at every parent level until
     * an element is found or the root level is reached.
     *
     * @return A stable pointer to the found element or `nullptr if the element does not exist at any scope level.
     * Stable means that future insertions, at any scope level, will not invalidate the returned pointer.
     */
    V* operator[](K const&);

    /** @brief Const-propagating overload, returning a const-pointer if the element is found. */
    V const* operator[](K const&) const;
};

// implementation

/** @brief Stores the internal state at a specific scope level and a pointer to the parent level. */
template <typename K, typename V>
struct scope_map<K, V>::state_t
{
    std::shared_ptr<state_t> parent;
    data_map data;
};

template <typename K, typename V>
scope_map<K, V>::scope_map() :
    state(std::make_shared<state_t>())
{ }

template <typename K, typename V>
scope_map<K, V>::scope_map(std::shared_ptr<state_t> p) :
    state(std::move(p))
{ }

template <typename K, typename V>
scope_map<K, V>::scope_map(scope_map const& that) :
    state(std::make_shared<state_t>(that.state->parent, that.state->data))
{ }

template <typename K, typename V>
auto scope_map<K, V>::operator=(scope_map const& that) -> scope_map&
{
    state = std::make_shared<state_t>(that.state.parent, that.state.data);
    return *this;
}

template <typename K, typename V>
auto scope_map<K, V>::begin() const -> const_iterator
{
    return state->data.begin();
}

template <typename K, typename V>
auto scope_map<K, V>::end() const -> const_iterator
{
    return state->data.end();
}

template <typename K, typename V>
auto scope_map<K, V>::extend() const -> scope_map
{
    return scope_map(std::make_shared<state_t>(state));
}

template <typename K, typename V>
auto scope_map<K, V>::parent() -> std::optional<scope_map>
{
    if (state->parent)
        return scope_map(state->parent);
    else
        return std::nullopt;
}

template <typename K, typename V>
auto scope_map<K, V>::parent() const -> std::optional<scope_map const>
{
    if (state->parent)
        return scope_map(state->parent);
    else
        return std::nullopt;
}

template <typename K, typename V>
template <typename... Args>
auto scope_map<K, V>::try_emplace(K const& k, Args&&... args) -> std::pair<iterator, bool>
{
    return state->data.try_emplace(k, std::forward<Args>(args)...);
}

template <typename K, typename V>
template <typename... Args>
auto scope_map<K, V>::try_emplace(K&& k, Args&&... args) -> std::pair<iterator, bool>
{
    return state->data.try_emplace(std::move(k), std::forward<Args>(args)...);
}

template <typename K, typename V>
V* scope_map<K, V>::operator[](K const& k)
{
    return const_cast<V*>(const_cast<scope_map const*>(this)->operator[](k));
}

template <typename K, typename V>
V const* scope_map<K, V>::operator[](K const& k) const
{
    auto const *s = state.get();
    do
    {
        if (auto const it = s->data.find(k); it != s->data.end())
            return &it->second;
        s = s->parent.get();
    }
    while (s);
    return nullptr;
}

} // namespace dep0
