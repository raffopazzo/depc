#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

namespace dep0::parser {

struct parse_tree_properties
{
    using module_properties_type = source_loc_t;
    using func_def_properties_type = source_loc_t;
    using type_properties_type = source_loc_t;
    using body_properties_type = source_loc_t;
    using stmt_properties_type = source_loc_t;
    using expr_properties_type = source_loc_t;
};
static_assert(ast::Properties<parse_tree_properties>);

using module_t = ast::module_t<parse_tree_properties>;
using func_def_t = ast::func_def_t<parse_tree_properties>;
using type_t = ast::type_t<parse_tree_properties>;
using body_t = ast::body_t<parse_tree_properties>;
using stmt_t = ast::stmt_t<parse_tree_properties>;
using expr_t = ast::expr_t<parse_tree_properties>;

} // namespace dep0::parser
