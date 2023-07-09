#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P>
struct builder_t
{
    using module_properties_type   = typename P::module_properties_type;
    using func_def_properties_type = typename P::func_def_properties_type;
    using type_properties_type     = typename P::type_properties_type;
    using body_properties_type     = typename P::body_properties_type;
    using stmt_properties_type     = typename P::stmt_properties_type;
    using expr_properties_type     = typename P::expr_properties_type;

    using module_t = ast::module_t<P>;
    using func_def_t = ast::func_def_t<P>;
    using type_t = ast::type_t<P>;
    using body_t = ast::body_t<P>;
    using stmt_t = ast::stmt_t<P>;
    using expr_t = ast::expr_t<P>;

    template <template <typename> typename T, typename... Args>
    requires(Node<T>)
    T<P> make(typename T<P>::properties_t properties, Args&&... args)
    {
        return T<P>{std::move(properties), std::forward<Args>(args)...};
    }

    template <typename... Args>
    module_t make_module(module_properties_type properties, Args&&... args)
    {
        return module_t{std::move(properties), std::forward<Args>(args)...};
    }
 
    template <typename... Args>
    func_def_t make_func_def(func_def_properties_type properties, Args&&... args)
    {
        return func_def_t{std::move(properties), std::forward<Args>(args)...};
    }
 
    template <typename... Args>
    type_t make_type(type_properties_type properties, Args&&... args)
    {
        return type_t{std::move(properties), std::forward<Args>(args)...};
    }

    template <typename... Args>
    body_t make_body(body_properties_type properties, Args&&... args)
    {
        return body_t{std::move(properties), std::forward<Args>(args)...};
    }
 
    template <typename... Args>
    stmt_t make_stmt(stmt_properties_type properties, Args&&... args)
    {
        return stmt_t{std::move(properties), std::forward<Args>(args)...};
    }

    stmt_t make_return(stmt_properties_type properties, expr_t expr)
    {
        return stmt_t{std::move(properties), typename stmt_t::return_t{std::move(expr)}};
    }

    template <typename... Args>
    expr_t make_expr(expr_properties_type properties, Args&&... args)
    {
        return expr_t{std::move(properties), std::forward<Args>(args)...};
    }

    expr_t make_numeric_constant(expr_properties_type properties, source_text number)
    {
        return expr_t{std::move(properties), typename expr_t::numeric_constant_t{std::move(number)}};
    }
};

} // namespace dep0::ast
