#pragma once

#include <memory>

namespace dep0 {

// Like `unique_ptr` but removes `nullptr` from its inhabitants.
template <typename T>
class unique_ref
{
    std::unique_ptr<T> ptr;

    explicit unique_ref(std::unique_ptr<T> p) : ptr(std::move(p)) {}

    template <typename U, typename... Args>
    friend unique_ref<U> make_ref(Args&&... args);

public:
    unique_ref(unique_ref&&) = default;
    unique_ref& operator=(unique_ref&&) = default;

    T* operator->() { return ptr.get(); }
    T& get() { return *ptr; }

    T const* operator->() const { return ptr.get(); }
    T const& get() const { return *ptr; }
};

template <typename T, typename... Args>
unique_ref<T> make_ref(Args&&... args)
{
      return unique_ref(std::make_unique<T>(std::forward<Args>(args)...));
}

} // namespace dep0
