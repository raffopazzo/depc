#include "private/mangler.hpp"

#include "dep0/typecheck/ast.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/match.hpp"

#include <sstream>

namespace dep0::llvmgen {

static std::ostream& mangled_print(std::ostream& os, typecheck::type_t const& t)
{
    match(
        t.value,
        [&] (typecheck::type_t::bool_t const&) { os << "bool"; },
        [&] (typecheck::type_t::unit_t const&) { os << "unit_t"; },
        [&] (typecheck::type_t::i8_t const&) { os << "i8_t"; },
        [&] (typecheck::type_t::i16_t const&) { os << "i16_t"; },
        [&] (typecheck::type_t::i32_t const&) { os << "i32_t"; },
        [&] (typecheck::type_t::i64_t const&) { os << "i64_t"; },
        [&] (typecheck::type_t::u8_t const&) { os << "u8_t"; },
        [&] (typecheck::type_t::u16_t const&) { os << "u16_t"; },
        [&] (typecheck::type_t::u32_t const&) { os << "u32_t"; },
        [&] (typecheck::type_t::u64_t const&) { os << "u64_t"; },
        [&] (typecheck::type_t::var_t const& x) { pretty_print(os, x.name); },
        [&] (typecheck::type_t::arr_t const& x)
        {
            os << '(';
            for (bool first = true; auto const& arg: x.args)
            {
                if (not std::exchange(first, false))
                    os << ',';
                match(
                    arg.value,
                    [&] (typecheck::func_arg_t::type_arg_t const&) { os << "typename"; },
                    [&] (typecheck::func_arg_t::term_arg_t const& x) { mangled_print(os, x.type); });
            }
            mangled_print(os << ')', x.ret_type.get());
        });
    return os;
}

std::string make_mangled_name(source_text const& name, std::vector<typecheck::type_t> const& types)
{
    std::ostringstream s;
    s << name;
    for (auto const& t: types)
        mangled_print(s << '.', t);
    return s.str();
}

} // namespace dep0::llvmgen
