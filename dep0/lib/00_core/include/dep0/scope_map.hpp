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

#include <algorithm>
#include <deque>
#include <iterator>
#include <limits>
#include <memory>
#include <map>
#include <deque>
#include <optional>
#include <vector>

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
    using data_map = std::deque<std::pair<K const, V>>;

    struct state_t;
    std::shared_ptr<state_t> state;

    explicit scope_map(std::shared_ptr<state_t>);

    enum class iterator_direction_t { forward, reverse };
public:
    template <typename Derived, iterator_direction_t direction>
    class levels_iterator_base : public std::iterator<std::input_iterator_tag, std::pair<K const, V>>
    {
        using underlying_t =
            std::conditional_t<
                direction == iterator_direction_t::reverse,
                typename data_map::const_reverse_iterator,
                typename data_map::const_iterator>;
    public:
        using levels_t = std::vector<std::pair<underlying_t, underlying_t>>;
    private:
        std::shared_ptr<levels_t> m_levels; // makes it cheap to copy iterators
        std::size_t m_current_level;
        underlying_t m_it;

        std::pair<underlying_t, underlying_t> const& current_level() const
        {
            assert(not is_end());
            return (*m_levels)[m_current_level];
        }
        bool is_end() const { return m_levels->empty() or m_current_level >= m_levels->size(); }

    public:
        levels_iterator_base() : levels_iterator_base(levels_t{}) {}
        explicit levels_iterator_base(levels_t levels)
            : m_levels(std::make_shared<levels_t>(std::move(levels)))
            , m_current_level(0ul)
            , m_it(m_levels->empty() ? underlying_t{} : current_level().first)
        { }

        std::pair<K const, V> const& operator*() const { return *m_it; }
        std::pair<K const, V> const* operator->() const { return &*m_it; }
        Derived& operator++()
        {
            assert(not is_end());
            assert(m_it != current_level().second);
            if (++m_it == current_level().second)
                if (++m_current_level < m_levels->size())
                    m_it = current_level().first;
            return static_cast<Derived&>(*this);
        }
        Derived operator++(int)
        {
            auto old = *this;
            ++(*this);
            return old;
        }
        bool operator!=(levels_iterator_base const& that) const { return not (*this == that); }
        bool operator==(levels_iterator_base const& that) const
        {
            if (is_end() and that.is_end())
                return true;
            if (is_end() xor that.is_end())
                return false;
            return m_it == that.m_it;
        }
    };
    struct forward_levels_iterator_t : levels_iterator_base<forward_levels_iterator_t, iterator_direction_t::forward>
    {
        using levels_iterator_base<forward_levels_iterator_t, iterator_direction_t::forward>::levels_iterator_base;
    };
    struct reverse_levels_iterator_t : levels_iterator_base<reverse_levels_iterator_t, iterator_direction_t::reverse>
    {
        using levels_iterator_base<reverse_levels_iterator_t, iterator_direction_t::reverse>::levels_iterator_base;
    };

    using iterator = typename data_map::iterator;
    using value_type = std::pair<K const, V>;

    scope_map();                                        /**< @brief Constructs an empty scope. */
    scope_map(scope_map const&) = delete;               /**< @brief Copying is expensive, use `copy()` instead. */
    scope_map& operator=(scope_map const&) = delete;    /**< @brief Copying is expensive, use `copy()` instead. */

    /**
     * @brief Constructs a copy that behaves like the original but separate from other copies.
     *
     * For example, adding a duplicate entry will fail.
     * But if you add a new entry to the copy, the original will not see it.
     *
     * Obviously you can also extend the new copy as normal.
     */
    scope_map copy() const;

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
     * @brief Iterator over all entries, from the outermost scope to the innermost and including all shadowed entries.
     *
     * Iteration begins from the very first entry in the outermost scope and proceeds in
     * insertion order until the most recent entry in the innermost scope.
     */
    forward_levels_iterator_t cbegin() const;
    forward_levels_iterator_t cend() const { return {}; }

    /**
     * @brief Like `cbegin()` but in reverse order from the most recent entry in the innermost scope,
     * proceeding in reverse insertion order until the very first entry in the outermost scope.
     */
    reverse_levels_iterator_t rbegin() const;
    reverse_levels_iterator_t rend() const { return {}; }

    /** @brief Iterator over all entries of the innermost scope only, in insertion order. */
    auto innermost_begin() const { return state->data.begin(); }
    auto innermost_end() const { return state->data.end(); }
    auto innermost_range() const { return std::ranges::subrange(innermost_begin(), innermost_end()); }

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
     * @brief Inserts a new key-value pair, if the key does not already exist at the current scope.
     *
     * Insertion succeeds if and only if there is no element with the given key at the current scope level,
     * even though there is one at the parent scope level, thereby shadowing the parent one.
     *
     * In any case, if insertion takes place the value is constructed in-place by forwarding all remaining arguments.
     *
     * @return The iterator to the key-value pair with a flag which is true if insertion took place.
     */
    template <typename... Args> std::pair<iterator, bool> try_emplace(K, Args&&...);

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
    std::size_t max_parent_index; // when looking into the parent, look only before this index
    data_map data;
    std::map<K, std::size_t> index;

    /** @brief Constructs the root scope. */
    state_t() : parent(nullptr), max_parent_index(std::numeric_limits<std::size_t>::max()) {}

    /** @brief Constructs a copy of the current scope, with the same parent. */
    state_t(state_t const&) = default;
    state_t& operator=(state_t const&) = default;

    // move could be implemented but unnecessary because we use shared_ptr
    state_t(state_t&&) = delete;
    state_t& operator=(state_t&&) = delete;

    /** @brief Constructs a new empty scope, that extends from the given parent. */
    state_t(std::shared_ptr<state_t> parent, std::size_t const max_parent_index)
        : parent(std::move(parent)), max_parent_index(max_parent_index)
    {}
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
scope_map<K, V> scope_map<K, V>::copy() const
{
    return scope_map(std::make_shared<state_t>(*state));
}

template <typename K, typename V>
auto scope_map<K, V>::cbegin() const -> forward_levels_iterator_t
{
    typename forward_levels_iterator_t::levels_t levels;
    auto const* s = state.get();
    auto max_index = std::numeric_limits<std::ptrdiff_t>::max();
    do
    {
        auto const safe_max = std::min(max_index, std::distance(s->data.cbegin(), s->data.cend()));
        auto const begin = s->data.cbegin();
        auto const end = std::next(s->data.cbegin(), safe_max);
        if (begin != end)
            levels.push_back(std::pair{s->data.cbegin(), std::next(s->data.cbegin(), safe_max)});
        max_index = s->max_parent_index;
        s = s->parent.get();
    }
    while (s);
    std::reverse(levels.begin(), levels.end());
    return forward_levels_iterator_t(std::move(levels));
}

template <typename K, typename V>
auto scope_map<K, V>::rbegin() const -> reverse_levels_iterator_t
{
    typename reverse_levels_iterator_t::levels_t levels;
    auto const* s = state.get();
    auto max_index = std::numeric_limits<std::ptrdiff_t>::max();
    do
    {
        auto const safe_max = std::min(max_index, std::distance(s->data.cbegin(), s->data.cend()));
        auto const begin = s->data.cbegin();
        auto const end = std::next(s->data.cbegin(), safe_max);
        if (begin != end)
            levels.push_back(std::pair{std::make_reverse_iterator(end), std::make_reverse_iterator(begin)});
        max_index = s->max_parent_index;
        s = s->parent.get();
    }
    while (s);
    return reverse_levels_iterator_t(std::move(levels));
}

template <typename K, typename V>
auto scope_map<K, V>::extend() const -> scope_map
{
    return scope_map(std::make_shared<state_t>(state, state ? state->data.size() : 0ul));
}

template <typename K, typename V>
template <typename... Args>
auto scope_map<K, V>::try_emplace(K k, Args&&... args) -> std::pair<iterator, bool>
{
    auto const [it_index, inserted] = state->index.try_emplace(k, state->data.size());
    if (inserted)
    {
        try { state->data.emplace_back(std::move(k), V{std::forward<Args>(args)...}); }
        catch (...)
        {
            state->index.erase(it_index);
            throw;
        }
    }
    return std::pair{std::next(state->data.begin(), it_index->second), inserted};
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
    auto max_index = std::numeric_limits<std::size_t>::max();
    do
    {
        if (auto const it = s->index.find(k); it != s->index.end() and it->second < max_index)
            return &s->data[it->second].second;
        max_index = s->max_parent_index;
        s = s->parent.get();
    }
    while (s);
    return nullptr;
}

} // namespace dep0
