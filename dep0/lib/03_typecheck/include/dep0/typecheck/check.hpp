#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"
#include "dep0/ast/tree.hpp"
#include "dep0/parser/parse_tree.hpp"

#include "dep0/error.hpp"

#include <algorithm>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace dep0::typecheck {

struct empty_t{};

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

using tree = ast::tree<ast::module_t, derivation_properties>; // TODO remove

expected<module_t> check(parser::module_t const&);
expected<func_def_t> check(parser::func_def_t const&);
expected<type_t> check(parser::type_t const&);
expected<body_t> check(parser::body_t const&, std::optional<type_t> const&);
expected<stmt_t> check(parser::stmt_t const&, std::optional<type_t> const&);
expected<expr_t> check(parser::expr_t const&, type_t const&);

expected<tree> check(parser::parse_tree const&);

} // namespace dep0::typecheck
