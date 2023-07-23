#include "dep0/typecheck/tt/type.hpp"

#include "dep0/match.hpp"

#include <stack>
#include <tuple>

namespace dep0::typecheck::tt {

bool type_t::arr_t::operator==(type_t::arr_t const& that) const
{
    return std::tie(dom.get(), img.get()) == std::tie(that.dom.get(), that.img.get());
}

type_t type_t::var(source_text name)
{
    return type_t(type_t::var_t(std::move(name)));
}

type_t type_t::arr(type_t dom, type_t img)
{
    return type_t(type_t::arr_t(std::move(dom), std::move(img)));
}

static std::pair<type_t, std::stack<type_t>> get_proto(type_t::arr_t const& f)
{
    return match(
        f.img.get().value,
        [&] (type_t::var_t const& x)
        {
            std::stack<type_t> args;
            args.push(f.dom.get());
            return std::make_pair(type_t(x), std::move(args));
        },
        [&] (type_t::arr_t const& g)
        {
            auto [g_ret, g_args] = get_proto(g);
            g_args.push(f.dom.get());
            return std::make_pair(g_ret, std::move(g_args));
        });
}

std::ostream& pretty_print(std::ostream& os, type_t const& ty)
{
    match(
        ty.value,
        [&] (type_t::var_t const& x)
        {
            os << x.name;
        },
        [&] (type_t::arr_t const& x)
        {
            auto [ret, arg_stack] = get_proto(x);
            bool first = true;
            os << '(';
            while (not arg_stack.empty())
            {
                pretty_print(std::exchange(first, false) ? os : os << ", ", arg_stack.top());
                arg_stack.pop();
            }
            pretty_print(os << ") -> ", ret);
        });
    return os;
}

} // namespace dep0::typecheck::tt
