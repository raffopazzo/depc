#include "dep0/typecheck/tt/type.hpp"

#include <tuple>

namespace dep0::typecheck::tt {

bool type_t::arr_t::operator==(type_t::arr_t const& that) const
{
    return std::tie(dom.get(), img.get()) == std::tie(that.dom.get(), that.img.get());
}

type_t type_t::var(std::string name)
{
    return type_t(type_t::var_t(std::move(name)));
}

type_t type_t::arr(type_t dom, type_t img)
{
    return type_t(type_t::arr_t(std::move(dom), std::move(img)));
}

std::ostream& pretty_print(std::ostream& os, type_t const& ty)
{
    struct visitor
    {
        std::ostream& os;
        std::ostream& operator()(type_t::var_t const& x) const
        {
            return os << x.name;
        }
        std::ostream& operator()(type_t::arr_t const& x) const
        {
            os << '(';
            std::visit(*this, x.dom.get().value);
            if (auto const* const p_var = std::get_if<type_t::var_t>(&x.img.get().value))
                os << ") -> " << p_var->name;
            else
            {
                os << ", ";
                std::visit(*this, x.img.get().value);
            }
            return os;
        }
    };
    return std::visit(visitor{os}, ty.value);
}

} // namespace dep0::typecheck::tt
