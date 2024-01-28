#include "private/gen_array.hpp"

#include "private/gen_val.hpp"

#include <numeric>

namespace dep0::llvmgen {

bool is_array(typecheck::expr_t const& type)
{
    auto const app = std::get_if<typecheck::expr_t::app_t>(&type.value);
    return app and std::holds_alternative<typecheck::expr_t::array_t>(app->func.get().value);
}

std::optional<array_properties_view_t> get_properties_if_array(typecheck::expr_t const& type)
{
    auto curr = get_if_app_of_array(type);
    if (not curr)
        return std::nullopt;
    std::vector<typecheck::expr_t const*> dimensions;
    dimensions.push_back(&curr->args[1ul]);
    while (auto const next = get_if_app_of_array(curr->args[0ul]))
    {
        dimensions.push_back(&next->args[1ul]);
        curr = next;
    }
    return array_properties_view_t{curr->args[0ul], std::move(dimensions)};
}

array_properties_view_t get_array_properties(typecheck::expr_t const& type)
{
    auto properties = get_properties_if_array(type);
    assert(properties.has_value() and "type must be an array");
    return std::move(*properties);
}

llvm::Value* gen_array_total_size(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    array_properties_view_t const& properties)
{
    assert(properties.dimensions.size() > 0ul);
    return std::accumulate(
        std::next(properties.dimensions.begin()), properties.dimensions.end(),
        gen_val(global, local, builder, *properties.dimensions[0ul], nullptr),
        [&] (llvm::Value* const acc, typecheck::expr_t const* size)
        {
            return builder.CreateMul(acc, gen_val(global, local, builder, *size, nullptr));
        });
}

llvm::Value* gen_stride_size_if_needed(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    array_properties_view_t const& properties)
{
    return
        properties.dimensions.size() > 1ul
        ? std::accumulate(
            std::next(properties.dimensions.begin(), 2ul), properties.dimensions.end(),
            gen_val(global, local, builder, *properties.dimensions[1ul], nullptr),
            [&] (llvm::Value* const acc, typecheck::expr_t const* size)
            {
                return builder.CreateMul(acc, gen_val(global, local, builder, *size, nullptr));
            })
        : nullptr;
}

} // namespace dep0::llvmgen


