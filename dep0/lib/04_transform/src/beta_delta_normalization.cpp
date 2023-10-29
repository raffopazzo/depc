#include "dep0/transform/beta_delta_normalization.hpp"

#include "dep0/ast/beta_delta_reduction.hpp"

namespace dep0::transform {

expected<std::true_type> beta_delta_normalization_t::operator()(typecheck::module_t& m) const
{
    beta_delta_normalize(ctx, m);
    return std::true_type{};
}

} // namespace dep0::transform

