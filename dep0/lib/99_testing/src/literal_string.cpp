#include "dep0/testing/literal_string.hpp"

namespace dep0::testing {

source_text literal_string(char const* s)
{
    return {make_handle<char const*>(s), s};
}

} // namespace dep0::testing
