#include "dep0/error.hpp"

namespace dep0 {

static bool is_single_line(std::string_view const text)
{
    return text.find('\n') == text.npos;
}

static std::ostream& print_indent(std::ostream& os, std::size_t indent)
{
    while (indent--)
        os << "    ";
    return os;
}

static std::ostream& new_line(std::ostream& os, std::size_t const indent)
{
    return print_indent(os << std::endl, indent);
}

static std::ostream& quote(std::ostream& os, std::string_view text, std::size_t const indent)
{
    auto nl = text.find('\n');
    while (nl != text.npos)
    {
        new_line(os, indent) << "> " << text.substr(0, nl);
        text.remove_prefix(nl + 1);
        nl = text.find('\n');
    }
    return new_line(os, indent) << "> " << text;
}

static std::ostream& without_indent(std::ostream& os, error_t const& err, std::size_t indent, std::size_t const reason);

static std::ostream& with_indent(std::ostream& os, error_t const& err, std::size_t indent, std::size_t const reason)
{
    print_indent(os, indent);
    return without_indent(os, err, indent, reason);
}

enum class quoting_mode
{
    dont_quote,
    single_line,
    multi_line
};

static quoting_mode determine_quoting_mode(std::optional<source_loc_t> const& loc)
{
    if (not loc or loc->txt.empty())
        return quoting_mode::dont_quote;
    return is_single_line(loc->txt) ? quoting_mode::single_line : quoting_mode::multi_line;
}

static std::ostream& without_indent(std::ostream& os, error_t const& err, std::size_t indent, std::size_t const reason)
{
    if (reason)
        os << reason << ". ";
    auto const q = determine_quoting_mode(err.location);
    if (err.location)
    {
        os << "at " << err.location->line << ':' << err.location->col;
        if (q == quoting_mode::single_line)
            os << " `" << err.location->txt << '`';
        os << ' ';
    }
    os << err.error;
    if (q == quoting_mode::multi_line)
        quote(os, err.location->txt, indent+1);
    auto const because = [&] () -> std::ostream&
    {
        if (q == quoting_mode::multi_line)
            new_line(os, indent);
        else
            os << ' ';
        return os << "because";
    };
    switch (err.reasons.size())
    {
    case 0ul: return os;
    case 1ul:
        because() << ' ';
        return without_indent(os, err.reasons[0], indent, 0ul);
    default:
        because() << ':';
        for (std::size_t i = 1ul; auto const& reason: err.reasons)
            with_indent(os << std::endl, reason, indent+1, i++);
        return os;
    }
}

std::ostream& pretty_print(std::ostream& os, error_t const& err)
{
    return without_indent(os, err, 0ul, 0ul);
}

} // namespace dep0
