#include "dep0/transform/reify_return_unit.hpp"

namespace dep0::transform {

expected<std::true_type> reify_return_unit(typecheck::module_t& m)
{
    for (auto& f: m.func_defs)
        if (std::holds_alternative<typecheck::type_t::unit_t>(f.type.value))
            if (f.body.stmts.empty() or not std::holds_alternative<typecheck::stmt_t::return_t>(f.body.stmts.back().value))
                f.body.stmts.push_back(typecheck::stmt_t{{}, typecheck::stmt_t::return_t{std::nullopt}});
    return std::true_type{};
}

} // namespace dep0::transform

