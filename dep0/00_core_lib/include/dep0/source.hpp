#pragma once

#include <any>
#include <cstdint>
#include <ostream>
#include <string_view>

namespace dep0 {

// Type-erased, single-threaded, reference-counted handle to some source dode, eg a mmap'd file or a string input.
// Conceptually similar to `std::shared_ptr<std::any>` but cheaper and removes `nullptr` from its inhabitants.
class source_handle_t
{
    struct state_t
    {
        std::size_t counter;
        std::any obj;
    };

    state_t* state = nullptr;

    template <typename T>
    source_handle_t(T x) :
        state(new state_t{1ul, std::move(x)})
    { }

    template <typename U, typename... Args>
    friend source_handle_t make_handle(Args&&... args);

    void release()
    {
        // on quick-bench this combination of likelihood attributes perfomed the best, which makes intuitive sense:
        // if we have a source file open, we really only close it once at the end so `delete` is twice unlikely,
        // especially if we always try to move handles
        if (state) [[unlikely]]
            if (0 == --state->counter) [[unlikely]]
                delete std::exchange(state, nullptr);
    }

public:
    ~source_handle_t()
    {
        release();
    }

    source_handle_t(source_handle_t const& that) :
        state(that.state)
    {
        ++that.state->counter;
    }

    source_handle_t(source_handle_t&& that) :
        state(std::exchange(that.state, nullptr))
    { }

    source_handle_t& operator=(source_handle_t const& that)
    {
        release();
        state = that.state;
        ++that.state->counter;
        return *this;
    }

    source_handle_t& operator=(source_handle_t&& that)
    {
        release();
        state = std::exchange(that.state, nullptr);
        return *this;
    }
};

template <typename U, typename... Args>
source_handle_t make_handle(Args&&... args)
{
    return source_handle_t(U{std::forward<Args>(args)...});
}


// Like `std::string_view` but guarantees that the source is still open.
struct source_text
{
    source_handle_t hdl;
    std::string_view txt;

    source_text substr(std::size_t const pos, std::size_t const n) const
    {
        return {hdl, txt.substr(pos, n)};
    }

    bool operator==(std::string_view const s) const { return txt == s; }
    bool operator!=(std::string_view const s) const { return txt == s; }

    bool operator==(source_text const& that) const { return txt == that.txt; }
    bool operator!=(source_text const& that) const { return txt == that.txt; }

};

inline std::ostream& operator<<(std::ostream& os, source_text const& s) { return os << s.txt; }

struct source_loc_t
{
    std::size_t line;
    std::size_t col;
    source_text txt;

    source_loc_t(std::size_t const line, std::size_t const col, source_text txt) :
        line(line), col(col), txt(std::move(txt))
    { }

    bool operator==(source_loc_t const&) const = default;
    bool operator!=(source_loc_t const&) const = default;
};

} // namespace dep0

