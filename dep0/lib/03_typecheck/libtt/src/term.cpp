#include "dep0/typecheck/tt/term.hpp"

#include "dep0/match.hpp"

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

term_t term_t::const_(std::size_t id, source_text value)
{
    return term_t(const_t(id, std::move(value)));
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

template <typename T>
std::ostream& with_parens(std::ostream& os, T const& x)
{
    return pretty_print(os << '(', x) << ')';
}

std::ostream& pretty_print(std::ostream& os, term_t const& x)
{
    match(
        x.value,
        [&] (term_t::const_t const& x) { os << x.value; },
        [&] (term_t::var_t const& x) { os << x.name; },
        [&] (term_t::app_t const& x)
        {
            auto const impl = [&] (term_t const& y)
            {
                if (is_var(y))
                    pretty_print(os, y);
                else
                    with_parens(os, y);
            };
            impl(x.left.get());
            if (is_var(x.left.get()) and is_var(x.right.get()))
                os << ' ';
            impl(x.right.get());
        },
        [&] (term_t::abs_t const& x)
        {
            os << "lambda " << x.var.name;
            auto const* body_ptr = &x.body;
            pretty_print(os << " : ", x.var_type);
            while (auto const* const p = std::get_if<term_t::abs_t>(&body_ptr->get().value))
            {
                os << ", " << p->var.name;
                pretty_print(os << " : ", p->var_type);
                body_ptr = &p->body;
            }
            pretty_print(os << " . ", body_ptr->get());
        });
    return os;
}

} // namespace dep0::typecheck::tt

