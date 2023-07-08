#pragma once

#include "dep0/ast/builder.hpp"

#include <algorithm>
#include <iterator>

namespace dep0::ast {

struct no_property
{
    bool operator==(no_property const&) const { return true; }
};

struct no_properties
{
    using module_properties_type = no_property;
    using func_def_properties_type = no_property;
    using type_properties_type = no_property;
    using body_properties_type = no_property;
    using stmt_properties_type = no_property;
    using expr_properties_type = no_property;
};

template <>
struct builder_t<no_properties>
{
    using module_properties_type = no_properties;
    using func_def_properties_type = no_properties;
    using type_properties_type = no_properties;
    using body_properties_type = no_properties;
    using stmt_properties_type = no_properties;
    using expr_properties_type = no_properties;

    using module_t = ast::module_t<no_properties>;
    using func_def_t = ast::func_def_t<no_properties>;
    using type_t = ast::type_t<no_properties>;
    using body_t = ast::body_t<no_properties>;
    using stmt_t = ast::stmt_t<no_properties>;
    using expr_t = ast::expr_t<no_properties>;

    template <template <typename> typename T, typename... Args>
    requires (Node<T>)
    T<no_properties> make(Args&&... args)
    {
        return T<no_properties>{{}, std::forward<Args>(args)...};
    }

    template <typename... Args>
    module_t make_module(Args&&... args)
    {
        return module_t{{}, std::forward<Args>(args)...};
    }
 
    template <typename... Args>
    func_def_t make_func_def(Args&&... args)
    {
        return func_def_t{{}, std::forward<Args>(args)...};
    }
 
    template <typename... Args>
    type_t make_type(Args&&... args)
    {
        return type_t{{}, std::forward<Args>(args)...};
    }
    template <typename... Args>
    type_t make_type_int()
    {
        return type_t{{}, type_t::int_t{}};
    }

    template <typename... Args>
    body_t make_body(Args&&... args)
    {
        return body_t{{}, std::forward<Args>(args)...};
    }
 
    template <typename... Args>
    stmt_t make_stmt(Args&&... args)
    {
        return stmt_t{{}, std::forward<Args>(args)...};
    }

    stmt_t make_return(expr_t expr)
    {
        return stmt_t{{}, stmt_t::return_t{std::move(expr)}};
    }
 
    template <typename... Args>
    expr_t make_expr(Args&&... args)
    {
        return expr_t{{}, std::forward<Args>(args)...};
    }

    expr_t make_numeric_constant(std::string_view const number)
    {
        return expr_t{{}, expr_t::numeric_constant_t{number}};
    }
};

using no_properties_builder = dep0::ast::builder_t<no_properties>;

} // namespace dep0::ast
