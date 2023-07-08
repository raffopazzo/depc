#pragma once

#include "dep0/ast/concepts.hpp"

#include "dep0/source.hpp"

namespace dep0::ast {

template <template <typename> typename Root, Properties P>
requires (Node<Root>)
struct tree
{
    using properties_type = P;
    using root_type = Root<P>;
    source_ref_t source;
    root_type root;
};

} // namespace dep0::ast
