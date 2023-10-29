#pragma once

#include "dep0/source.hpp"

namespace dep0::testing {

// Create a `source_text` from a global literal string.
// DO NOT use with runtime generated string or tests will eventually crash.
source_text literal_string(char const*);

} // namespace dep0::testing
