/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::unique_ref`.
 */
#pragma once

#include <memory>

namespace dep0 {

/** @brief Like `std::unique_ptr` but removes `nullptr` from its inhabitants. */
template <typename T>
class unique_ref // TODO use something from boost/std if/when available
{
    std::unique_ptr<T> ptr;

    explicit unique_ref(std::unique_ptr<T> p) : ptr(std::move(p)) {}

    template <typename U, typename... Args>
    friend unique_ref<U> make_ref(Args&&... args);

public:
    unique_ref(unique_ref&&) = default;
    unique_ref& operator=(unique_ref&&) = default;

    T* operator->() { return ptr.get(); }
    T& operator*() { return *ptr; }
    T& get() { return *ptr; }

    T const* operator->() const { return ptr.get(); }
    T const& operator*() const { return *ptr; }
    T const& get() const { return *ptr; }
};

/** @brief Constructs a `unique_ref` in-place by passing all the given arguments to the element's constructor. */
template <typename T, typename... Args>
unique_ref<T> make_ref(Args&&... args)
{
      return unique_ref(std::make_unique<T>(std::forward<Args>(args)...));
}

} // namespace dep0
