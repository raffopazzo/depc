#pragma once

namespace dep0::ast {

template <template <typename> typename T>
concept Node = true;

template <typename T>
concept Properties = requires(T)
{
    typename T::module_properties_type;
    typename T::type_def_properties_type;
    typename T::func_def_properties_type;
    typename T::func_call_properties_type;
    typename T::type_properties_type;
    typename T::body_properties_type;
    typename T::stmt_properties_type;
    typename T::expr_properties_type;
};

} // namespace dep0::ast
