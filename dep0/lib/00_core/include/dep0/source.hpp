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

    source_handle_t(std::nullptr_t);

    template <typename T>
    source_handle_t(T x) :
        state(new state_t{1ul, std::move(x)})
    { }

    template <typename U, typename... Args>
    friend source_handle_t make_handle(Args&&... args);

    friend source_handle_t make_null_handle();

    void release();

public:
    ~source_handle_t();
    source_handle_t(source_handle_t const&);
    source_handle_t(source_handle_t&&);
    source_handle_t& operator=(source_handle_t const&);
    source_handle_t& operator=(source_handle_t&&);
};

template <typename U, typename... Args>
source_handle_t make_handle(Args&&... args)
{
    return source_handle_t(U{std::forward<Args>(args)...});
}

source_handle_t make_null_handle();

// Like `std::string_view` but guarantees that the source is still open.
// Objects constructed using `from_literal` don't manage any memory
// since they are supposed to be passed a global literal string "...".
struct source_text
{
    source_handle_t hdl;
    std::string_view txt;

    source_text(source_handle_t, std::string_view);

    source_text substr(std::size_t pos, std::size_t n = std::string_view::npos) const;

    static source_text from_literal(char const*);

    std::string_view view() const;

    bool operator<(source_text const& that) const;
    bool operator==(source_text const& that) const;
    bool operator!=(source_text const& that) const;
};

bool operator==(source_text const&, std::string_view);
bool operator!=(source_text const&, std::string_view);
bool operator==(std::string_view, source_text const&);
bool operator!=(std::string_view, source_text const&);

std::ostream& operator<<(std::ostream&, source_text const&);

struct source_loc_t
{
    std::size_t line;
    std::size_t col;
    source_text txt;

    source_loc_t(std::size_t line, std::size_t col, source_text);

    bool operator==(source_loc_t const&) const = default;
    bool operator!=(source_loc_t const&) const = default;
};

} // namespace dep0

