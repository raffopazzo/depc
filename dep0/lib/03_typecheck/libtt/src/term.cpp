#include "dep0/typecheck/tt/term.hpp"

#include <tuple>

namespace dep0::typecheck::tt {

bool term_t::var_t::operator<(var_t const& that) const
{
    return name < that.name;
}

bool term_t::var_t::operator==(var_t const& that) const
{
    return name == that.name;
}

bool term_t::app_t::operator==(app_t const& that) const
{
    return std::tie(left.get(), right.get()) == std::tie(that.left.get(), that.right.get());
}

bool term_t::abs_t::operator==(abs_t const& that) const
{
    return std::tie(var, var_type, body.get()) == std::tie(that.var, that.var_type, that.body.get());
}

term_t term_t::var(source_text name)
{
    return term_t(var_t(std::move(name)));
}

term_t term_t::app(term_t left, term_t right)
{
    return term_t(app_t(std::move(left), std::move(right)));
}

term_t term_t::abs(source_text var_name, type_t var_type, term_t body)
{
    return term_t(abs_t(var_t(std::move(var_name)), std::move(var_type), std::move(body)));
}

term_t term_t::abs(var_t var_name, type_t var_type, term_t body)
{
    return term_t(abs_t(std::move(var_name), std::move(var_type), std::move(body)));
}

} // namespace dep0::typecheck::tt

