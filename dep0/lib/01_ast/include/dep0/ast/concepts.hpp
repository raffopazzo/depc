#pragma once

namespace dep0::ast {

/**
 * Checks whether `T` is a trait-type providing the type of properties for each node of the AST.
 */
template <typename T>
concept Properties = requires(T)
{
    typename T::module_properties_type;
    typename T::type_def_properties_type;
    typename T::func_def_properties_type;
    typename T::func_arg_properties_type;
    typename T::body_properties_type;
    typename T::stmt_properties_type;
    typename T::expr_properties_type;
};

} // namespace dep0::ast
