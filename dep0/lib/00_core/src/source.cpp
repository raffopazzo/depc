#include "dep0/source.hpp"

namespace dep0 {

// implementation of source_handle_t

source_handle_t::source_handle_t(std::nullptr_t) : state(nullptr) { }

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
source_handle_t::source_handle_t(source_handle_t const& that) : state(that.state) { ++that.state->counter; }
source_handle_t::source_handle_t(source_handle_t&& that) : state(std::exchange(that.state, nullptr)) { }

source_handle_t& source_handle_t::operator=(source_handle_t const& that)
{
    release();
    state = that.state;
    ++that.state->counter;
    return *this;
}

source_handle_t& source_handle_t::operator=(source_handle_t&& that)
{
    release();
    state = std::exchange(that.state, nullptr);
    return *this;
}

source_handle_t make_null_handle() { return source_handle_t(nullptr); }

// implementation of source_text

source_text::source_text(source_handle_t hdl, std::string_view txt) : hdl(std::move(hdl)), txt(txt) { }
source_text source_text::substr(std::size_t const pos, std::size_t const n) const { return {hdl, txt.substr(pos, n)}; }
source_text source_text::from_literal(char const* s) { return {make_handle<char const*>(s), s}; }
std::string_view source_text::view() const { return txt; }
bool source_text::operator<(source_text const& that) const { return view() < that.view(); }
bool source_text::operator==(source_text const& that) const { return view() == that.view(); }
bool source_text::operator!=(source_text const& that) const { return view() != that.view(); }

bool operator==(source_text const& x, std::string_view const s) { return x.view() == s; }
bool operator!=(source_text const& x, std::string_view const s) { return x.view() != s; }
bool operator==(std::string_view const s, source_text const& x) { return x.view() == s; }
bool operator!=(std::string_view const s, source_text const& x) { return x.view() != s; }

std::ostream& operator<<(std::ostream& os, source_text const& s) { return os << s.view(); }

source_loc_t::source_loc_t(std::size_t const line, std::size_t const col, source_text txt) :
    line(line), col(col), txt(std::move(txt))
{ }

} // namespace dep0
