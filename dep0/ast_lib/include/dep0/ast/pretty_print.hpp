#pragma once

#include "dep0/ast/ast.hpp"

#include <ostream>

namespace dep0::ast {

template <Properties P> std::ostream& pretty_print(std::ostream&, module_t<P> const&, std::size_t indent = 0ul);
template <Properties P> std::ostream& pretty_print(std::ostream&, func_def_t<P> const&, std::size_t indent = 0ul);
template <Properties P> std::ostream& pretty_print(std::ostream&, type_t<P> const&, std::size_t indent = 0ul);
template <Properties P> std::ostream& pretty_print(std::ostream&, body_t<P> const&, std::size_t indent = 0ul);
template <Properties P> std::ostream& pretty_print(std::ostream&, stmt_t<P> const&, std::size_t indent = 0ul);
template <Properties P> std::ostream& pretty_print(std::ostream&, expr_t<P> const&, std::size_t indent = 0ul);

inline std::ostream& pretty_indent(std::ostream& os, std::size_t indent)
{
    while (indent--)
        os << "    ";
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, module_t<P> const& x, std::size_t indent)
{
    for (auto const& f: x.func_defs)
        pretty_print(os, f, indent) << std::endl;
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, func_def_t<P> const& x, std::size_t indent)
{
    pretty_indent(os, indent);
    pretty_print(os, x.type) << ' ' << x.name << "()" << std::endl;
    return pretty_print(os, x.body, indent);
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, type_t<P> const& x, std::size_t indent)
{
    struct visitor
    {
        std::ostream& os;
        std::ostream& operator()(typename type_t<P>::int_t const& x)
        {
            return os << "int";
        }
    };
    std::visit(visitor{os}, x.value);
    return os;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, body_t<P> const& x, std::size_t indent)
{
    pretty_indent(os, indent++) << '{' << std::endl;
    for (auto const& s: x.stmts)
        pretty_print(pretty_indent(os, indent), s, indent) << std::endl;
    return pretty_indent(os, --indent) << '}' << std::endl;
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, stmt_t<P> const& x, std::size_t indent)
{
    struct visitor
    {
        std::ostream& os;
        std::size_t const indent;
        std::ostream& operator()(typename stmt_t<P>::return_t const& x)
        {
            return pretty_print(os << "return ", x.expr, indent);
        }
    };
    return std::visit(visitor{os, indent}, x.value) << ';';
}

template <Properties P>
std::ostream& pretty_print(std::ostream& os, expr_t<P> const& x, std::size_t indent)
{
    struct visitor
    {
        std::ostream& os;
        std::ostream& operator()(typename expr_t<P>::numeric_constant_t const& x)
        {
            return os << x.number;
        }
    };
    std::visit(visitor{os}, x.value);
    return os;
}

}
