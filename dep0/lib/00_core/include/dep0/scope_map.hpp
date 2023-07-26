#pragma once

#include "dep0/source.hpp"

#include <memory>
#include <map>
#include <optional>

namespace dep0 {

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
    using const_iterator = typename data_map::const_iterator;

    // A default constructed scope is empty and you can move it, obviously.
    // If you copy it, the new instance behaves like the original,
    // for instance adding a duplicate entry will fail.
    // But if you add a new entry the original will not see it.
    // Obviously you can also extend the new copy as normal.
    scope_map();
    scope_map(scope_map const&);
    scope_map& operator=(scope_map const&);
    scope_map(scope_map&&) = default;
    scope_map& operator=(scope_map&&) = default;

    using iterator = typename data_map::iterator;

    const_iterator begin() const;
    const_iterator end() const;

    // Create a new scope that extend the current one, where new declarations can be added shadowing old ones.
    scope_map extend() const;

    std::optional<scope_map> parent();
    std::optional<scope_map const> parent() const;

    // Try to insert an element with the given key.
    // Insertion succeeds iff there is no element with the given key at the current scope level,
    // even though there is one at the parent scope level, thereby shadowing the parent one.
    template <typename... Args> std::pair<iterator, bool> try_emplace(K const&, Args&&...);
    template <typename... Args> std::pair<iterator, bool> try_emplace(K&&, Args&&...);

    // Find an element by the given key, either at the current scope level or any parent level.
    // Return nullptr if the element does not exist at any scope level or a (stable) pointer to the found element.
    V* operator[](K const&);
    V const* operator[](K const&) const;
};

// implementation

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
