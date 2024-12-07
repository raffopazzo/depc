/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Facilities to manipulate a source file.
 */
#pragma once

#include <any>
#include <cstdint>
#include <ostream>
#include <string_view>

namespace dep0 {

/**
 * @brief Type-erased, single-threaded, reference-counted handle to some source code.
 *
 * For example an mmap'd file or a runtime-generated string.
 *
 * @remarks
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
    friend source_handle_t make_source_handle(Args&&... args);

    void acquire(state_t*);
    void release();

public:
    /**
     * @brief A tag type used by `source_text` whenever the source code is a compile-time C-string literal.
     *
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
 * @brief Construct a handle to some source code buffer.
 * 
 * For example `make_source_handle<std::string>("...")` for some runtime-generated source code,
 * or `make_source_handle<boost::iostreams::mapped_file_source>(...)` for an mmap'd file.
 */
template <typename U, typename... Args>
source_handle_t make_source_handle(Args&&... args)
{
    return source_handle_t(U{std::forward<Args>(args)...});
}

/**
 * @brief A piece of text from some source code.
 *
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
     * @brief Helper method to construct a source text backed by a literal C-string.
     *
     * @warning It is undefined behaviour to pass a string that is not a literal C-string.
     */
    static source_text from_literal(char const*);

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

/**
 * @brief The location of some source code snippet.
 * 
 * Keeps track of line and column number alongside the particular snippet of code located there.
 * In this context what constitutes a "snippet" is user-defined.
 * For example at line 1, column 1 you can have `func f() -> i32_t`;
 * the snippet of interest might be the keyword `func` or the whole declaration.
 */
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

