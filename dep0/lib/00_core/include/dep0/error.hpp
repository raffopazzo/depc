/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Helper types that allow to return descriptive error objects from functions that might fail.
 */
#pragma once

#include "dep0/source.hpp"

#include <optional>
#include <string>
#include <vector>
#include <variant>

namespace dep0 {

/**
 * @brief Bundles an error message together with its underlying reasons and the location in the user programme.
 *
 * In simple cases the vector of reasons can be empty, for example if a file does not exist.
 * In more complex cases the vector of reasons might form a tree;
 * for example typechecking failed because:
 * 
 * - the left-hand operand has the wrong type
 * - the right-hand operand failed to typecheck because:
 *   - this happened because:
 *     - ...
 *   - and that also happened.
 *
 * @see expected
 */
struct error_t
{
    std::string error; /**< @brief User-friendly error message. */
    std::optional<source_loc_t> location; /**< @brief Empty if the error is not directly linked to the user programme */
    std::vector<error_t> reasons; /**< @brief Possibly empty tree of underlying errors that lead to this error. */

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

/**
 * @brief Prints an error to the given stream with pretty indendation and block-quoting for easier reading.
 * 
 * If `location` is available this function prints line and column numbers along with the corresponding code snippet.
 * 
 * If there are multiple underlying reasons, each of them is printed with extra indentation.
 * 
 * @return The output stream to allow method-chaining, like `pretty_print(std::cerr, err) << std::endl;`
 */
std::ostream& pretty_print(std::ostream&, error_t const&);

// TODO use `std::expected` once available
/** @brief Poor man's version of `std::expected`, to be replaced once we upgrade compiler. */
template <typename T, typename E = error_t>
class [[nodiscard]] expected
{
    std::variant<T , E> value_;

public:
    using value_type = T;
    using error_type = E;

    expected() = default;

    /*implicit*/ expected(T v) : value_(std::move(v)) { }
    /*implicit*/ expected(E e) : value_(std::move(e)) { }

    template <typename U>
    /*implicit*/ expected(expected<U, E> that)
        : value_(
            that
            ? std::variant<T , E>{std::in_place_type<T>, std::move(*that)}
            : std::variant<T , E>{std::in_place_type<E>, std::move(that.error())})
    { }

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
