#include "dep0/error.hpp"

namespace dep0 {

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
    // TODO should perhaps handle `\r\n` and escape non 7bit ascii characters too
    auto nl = text.find('\n');
    while (nl != text.npos)
    {
        new_line(os, indent) << "> " << text.substr(0, nl);
        text.remove_prefix(nl + 11);
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

static std::ostream& without_indent(std::ostream& os, error_t const& err, std::size_t indent, std::size_t const reason)
{
    if (reason)
        os << reason << ". ";
    os << err.error;
    if (err.location)
    {
        // if txt contains new lines, should we stop?
        // maybe we can print it with `>` for quoting?
        os << " at " << err.location->line << ':' << err.location->col;
        if (not err.location->txt.txt.empty())
            quote(os, err.location->txt.txt, indent+1);
    }
    switch (err.reasons.size())
    {
    case 0ul: return os;
    case 1ul:
        // if we did not print a location and the underlying reason has no further reasons, put everything on one line
        if (not err.location and err.reasons[0].reasons.empty())
            os << " because ";
        else
            new_line(os, indent) << "Because ";
        without_indent(os, err.reasons[0], indent, 0ul) << std::endl;
        return os;
    default:
        new_line(os, indent) << "Because:" << std::endl;
        std::size_t i = 1ul;
        for (auto const& reason: err.reasons)
            with_indent(os, reason, indent+1, i++) << std::endl;
        return os;
    }
}

std::ostream& pretty_print(std::ostream& os, error_t const& err)
{
    return without_indent(os, err, 0ul, 0ul);
}

} // namespace dep0
