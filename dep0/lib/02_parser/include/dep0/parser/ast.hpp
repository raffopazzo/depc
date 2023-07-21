#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

namespace dep0::parser {

struct properties_t
{
    using module_properties_type = source_loc_t;
    using type_def_properties_type = source_loc_t;
    using func_def_properties_type = source_loc_t;
    using type_properties_type = source_loc_t;
    using body_properties_type = source_loc_t;
    using stmt_properties_type = source_loc_t;
    using expr_properties_type = source_loc_t;
};
static_assert(ast::Properties<properties_t>);

using module_t = ast::module_t<properties_t>;
using type_def_t = ast::type_def_t<properties_t>;
using func_def_t = ast::func_def_t<properties_t>;
using type_t = ast::type_t<properties_t>;
using body_t = ast::body_t<properties_t>;
using stmt_t = ast::stmt_t<properties_t>;
using expr_t = ast::expr_t<properties_t>;

} // namespace dep0::parser
