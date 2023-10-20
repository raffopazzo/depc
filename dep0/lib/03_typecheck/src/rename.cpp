#include "dep0/typecheck/rename.hpp"
#include "dep0/typecheck/replace.hpp"

#include "dep0/match.hpp"

#include <numeric>

namespace dep0::typecheck {

static std::size_t max_index(type_t const&);
static std::size_t max_index(
    type_t::arr_t::arg_kinds_const_iterator begin,
    type_t::arr_t::arg_kinds_const_iterator end,
    type_t const& ret_type);

type_t::var_t rename(
    type_t::var_t const& var,
    type_t::arr_t::arg_kinds_iterator const begin,
    type_t::arr_t::arg_kinds_iterator const end,
    type_t& ret_type)
{
    auto const max_idx = std::max(var.name.idx, max_index(begin, end, ret_type));
    auto const new_var = type_t::var_t{ast::indexed_var_t{var.name.txt, max_idx + 1ul}};
    replace(var, new_var, begin, end, ret_type);
    return new_var;
}

std::size_t max_index(
    type_t::arr_t::arg_kinds_const_iterator const begin,
    type_t::arr_t::arg_kinds_const_iterator const end,
    type_t const& ret_type)
{
    return std::accumulate(
        begin, end,
        max_index(ret_type),
        [] (std::size_t const acc, auto const& kind)
        {
            return std::max(
                acc,
                match(
                    kind,
                    [] (type_t::var_t const& v) { return v.name.idx; },
                    [] (type_t const& t) { return max_index(t); }));
        });
}

std::size_t max_index(type_t const& type)
{
    return match(
        type.value,
        [] (type_t::bool_t const&) { return 0ul; },
        [] (type_t::unit_t const&) { return 0ul; },
        [] (type_t::i8_t const&) { return 0ul; },
        [] (type_t::i16_t const&) { return 0ul; },
        [] (type_t::i32_t const&) { return 0ul; },
        [] (type_t::i64_t const&) { return 0ul; },
        [] (type_t::u8_t const&) { return 0ul; },
        [] (type_t::u16_t const&) { return 0ul; },
        [] (type_t::u32_t const&) { return 0ul; },
        [] (type_t::u64_t const&) { return 0ul; },
        [] (type_t::var_t const& x) { return x.name.idx; },
        [] (type_t::arr_t const& x) { return max_index(x.arg_kinds.begin(), x.arg_kinds.end(), x.ret_type.get()); });
}

} // namespace dep0::typecheck
