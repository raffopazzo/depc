#include "dep0/typecheck/tt/type.hpp"

#include "dep0/match.hpp"

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
            os << '(';
            pretty_print(os, x.dom.get());
            if (auto const* const p_var = std::get_if<type_t::var_t>(&x.img.get().value))
                os << ") -> " << p_var->name;
            else
                pretty_print(os << ", ", x.img.get());
        });
    return os;
}

} // namespace dep0::typecheck::tt
