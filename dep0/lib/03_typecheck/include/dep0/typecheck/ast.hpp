#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include <optional>

namespace dep0::typecheck {

struct empty_t{};

// TODO should be `legal_xxx_t` and remove empty_t
struct derivation
{
    // TODO: here we should store the computed type for the function/expression (maybe statement?)
    // also the derivation of type declarations somehow

    // currently however we only support `int`, so a value of type `derivation` represents a valid integer expression
};

struct derivation_properties
{
    using module_properties_type = empty_t;
    using func_def_properties_type = empty_t; // `func_def` carries its return type which contains its own derivation
    using type_properties_type = derivation;
    using body_properties_type = empty_t;
    using stmt_properties_type = std::optional<derivation>; // why? does it really make sense? worth adding a comment
    using expr_properties_type = derivation;
};
static_assert(ast::Properties<derivation_properties>);

using module_t = ast::module_t<derivation_properties>;
using func_def_t = ast::func_def_t<derivation_properties>;
using type_t = ast::type_t<derivation_properties>;
using body_t = ast::body_t<derivation_properties>;
using stmt_t = ast::stmt_t<derivation_properties>;
using expr_t = ast::expr_t<derivation_properties>;

} // namespace dep0::typecheck
