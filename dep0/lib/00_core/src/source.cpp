#include "dep0/source.hpp"

namespace dep0 {

// implementation of source_handle_t

source_handle_t::source_handle_t(source_handle_t::literal_string_tag_t) : state(nullptr) { }

void source_handle_t::acquire(state_t* const s)
{
    state = s;
    if (state)
        ++state->counter;
}

void source_handle_t::release()
{
    // on quick-bench this combination of likelihood attributes perfomed the best, which makes intuitive sense:
    // if we have a source file open, we really only close it once at the end so `delete` is twice unlikely,
    // especially if we always try to move handles
    if (state) [[unlikely]]
        if (0 == --state->counter) [[unlikely]]
            delete std::exchange(state, nullptr);
}

source_handle_t::~source_handle_t() { release(); }
source_handle_t::source_handle_t(source_handle_t const& that) { acquire(that.state); }
source_handle_t::source_handle_t(source_handle_t&& that) : state(std::exchange(that.state, nullptr)) { }

source_handle_t& source_handle_t::operator=(source_handle_t const& that)
{
    release();
    acquire(that.state);
    return *this;
}

source_handle_t& source_handle_t::operator=(source_handle_t&& that)
{
    release();
    state = std::exchange(that.state, nullptr);
    return *this;
}

// implementation of source_text

source_text source_text::from_literal(std::string_view const txt)
{
    return source_text(source_handle_t(source_handle_t::literal_string_tag_t{}), txt);
}

source_text::source_text(source_handle_t hdl, std::string_view const txt) :
    hdl(std::move(hdl)),
    txt(txt)
{ }

source_text::operator std::string_view&() { return txt; }
source_text::operator std::string_view const&() const { return txt; }
std::string_view const& source_text::view() const { return txt; }

std::size_t source_text::size() const { return txt.size(); }
bool source_text::empty() const { return txt.empty(); }
bool source_text::starts_with(char const c) const { return txt.starts_with(c); }

source_text source_text::substr(std::size_t const pos, std::size_t const n) const
{
    return {hdl, txt.substr(pos, n)};
}

bool source_text::operator<(source_text const& that) const { return txt < that.txt; }
bool source_text::operator==(source_text const& that) const { return txt == that.txt; }
bool source_text::operator!=(source_text const& that) const { return txt != that.txt; }

bool operator==(source_text const& x, std::string_view const s) { return x.txt == s; }
bool operator!=(source_text const& x, std::string_view const s) { return x.txt != s; }
bool operator==(std::string_view const s, source_text const& x) { return x.txt == s; }
bool operator!=(std::string_view const s, source_text const& x) { return x.txt != s; }

std::ostream& operator<<(std::ostream& os, source_text const& s) { return os << s.txt; }

source_loc_t::source_loc_t(std::size_t const line, std::size_t const col, source_text txt) :
    line(line), col(col), txt(std::move(txt))
{ }

} // namespace dep0
