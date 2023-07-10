#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/parser/ast.hpp"

#include "dep0/error.hpp"

#include <optional>

namespace dep0::typecheck {

expected<module_t> check(parser::module_t const&);
expected<func_def_t> check(parser::func_def_t const&);
expected<type_t> check(parser::type_t const&);
expected<body_t> check(parser::body_t const&, std::optional<type_t> const&);
expected<stmt_t> check(parser::stmt_t const&, std::optional<type_t> const&);
expected<expr_t> check(parser::expr_t const&, type_t const&);

} // namespace dep0::typecheck
