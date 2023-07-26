#pragma once

#include "dep0/source.hpp"

#include <optional>
#include <string>
#include <vector>
#include <variant>

namespace dep0 {

struct error_t
{
    std::string error;
    std::optional<source_loc_t> location;
    std::vector<error_t> reasons;

    error_t() = default;

    explicit error_t(std::string error, std::optional<source_loc_t> location = {}, std::vector<error_t> reasons = {}) :
        error(std::move(error)),
        location(std::move(location)),
        reasons(std::move(reasons))
    { }

    error_t(std::string error, std::vector<error_t> reasons) :
        error(std::move(error)),
        reasons(std::move(reasons))
    { }

    bool operator==(error_t const&) const = default;
    bool operator!=(error_t const&) const = default;
};

std::ostream& pretty_print(std::ostream& os, error_t const&);

// TODO use `std::expected` once available
template <typename T, typename E = error_t>
class [[nodiscard]] expected
{
    std::variant<T , E> value_;

public:
    using value_type = T;
    using error_type = E;

    expected() = default;

    /*implicit*/expected(T v) : value_(std::move(v)) { }
    /*implicit*/expected(E e) : value_(std::move(e)) { }

    template <typename... Args>
    expected(std::in_place_t, Args&&... args) :
        value_(std::in_place_type<T>, std::forward<Args>(args)...)
    { }

    expected(expected const&) = default;
    expected(expected&&) = default;
    expected& operator=(expected const&) = default;
    expected& operator=(expected&&) = default;

    bool operator==(expected const&) const = default;
    bool operator!=(expected const&) const = default;

    explicit operator bool() const { return has_value(); }
    bool has_value() const { return std::holds_alternative<T>(value_); }
    bool has_error() const { return std::holds_alternative<E>(value_); }

    T* operator->() { return std::get_if<T>(&value_); }
    T& operator*() { return *std::get_if<T>(&value_); }
    T& value() { return std::get<T>(value_); }
    E& error() { return std::get<E>(value_); }

    T const* operator->() const { return std::get_if<T>(&value_); }
    T const& operator*() const { return *std::get_if<T>(&value_); }
    T const& value() const { return std::get<T>(value_); }
    E const& error() const { return std::get<E>(value_); }
};

} // namespace dep0
