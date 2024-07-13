#pragma once

#include <any>
#include <cstdint>
#include <ostream>
#include <string_view>

namespace dep0 {

/**
 * Type-erased, single-threaded, reference-counted handle to some source code,
 * for example an mmap'd file or a runtime-generated string.
 * It is conceptually similar to `std::shared_ptr<std::any>`,
 * but cheaper and removes `nullptr` from its inhabitants,
 * so you never have to test `if (hdl)` for example.
 */
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

    void acquire(state_t*);
    void release();

public:
    /**
     * A tag type used by `source_text` whenever the source code is a compile-time C-string literal.
     * In this case there is no state to keep around, which might allow further optimisations.
     */
    class literal_string_tag_t
    {
        literal_string_tag_t() = default;
        friend class source_text;
    };

    ~source_handle_t();
    explicit source_handle_t(literal_string_tag_t);
    source_handle_t(source_handle_t const&);
    source_handle_t(source_handle_t&&);
    source_handle_t& operator=(source_handle_t const&);
    source_handle_t& operator=(source_handle_t&&);

};

/**
 * Construct a handle to some source code buffer,
 * for example `make_handle<std::string>("...")` for some runtime-generated source code,
 * or `make_handle<boost::iostreams::mapped_file_source>(...)` for an mmap'd file.
 * Instances of `source_text` act as a view into the source code keeping the underlying buffer alive.
 */
template <typename U, typename... Args>
source_handle_t make_handle(Args&&... args)
{
    return source_handle_t(U{std::forward<Args>(args)...});
}

/**
 * A piece of text from some source code.
 * Taking copies is very cheap so it is encouraged to pass by-value.
 * This is conceptually similar to a `std::string_view` but it also
 * guarantees that the underlying buffer of the source code is kept alive.
 */
class source_text
{
    source_handle_t hdl;
    std::string_view txt;

public:

    /**
     * Helper method to construct a source text backed by a literal C-string.
     *
     * @remarks It is undefined behaviour to pass a string that is not a literal C-string.
     */
    template <std::size_t N> requires (N > 0)
    static source_text from_literal(char const(&s)[N])
    {
        return source_text(source_handle_t(source_handle_t::literal_string_tag_t{}), std::string_view(s, N-1));
    }

    source_text(source_handle_t, std::string_view);
    source_text(source_text const&) = default;
    source_text(source_text&&) = default;
    source_text& operator=(source_text const&) = default;
    source_text& operator=(source_text&&) = default;
    operator std::string_view&(); // allows functions that take `std::string_view&` to modify `txt`
    operator std::string_view const&() const; // allows that perform tests on `std::string_view const&` to use `txt`
    std::string_view const& view() const;

    // WARN do not expose `char const* data() const` because our underlying buffer is the entire source file,
    // which is not null-terminated where you want, in fact it might not even be null-terminated at all!

    std::size_t size() const;
    bool empty() const;
    bool starts_with(char) const;
    source_text substr(std::size_t pos, std::size_t n = std::string_view::npos) const;

    bool operator<(source_text const&) const;
    bool operator==(source_text const&) const;
    bool operator!=(source_text const&) const;

    friend bool operator==(source_text const&, std::string_view);
    friend bool operator!=(source_text const&, std::string_view);
    friend bool operator==(std::string_view, source_text const&);
    friend bool operator!=(std::string_view, source_text const&);
    friend std::ostream& operator<<(std::ostream&, source_text const&);
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

