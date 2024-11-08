#pragma once

namespace dep0::ast {

/** Represents the bit width used inside a user-defined integral type definition. */
enum class width_t
{
    _8,
    _16,
    _32,
    _64
};

bool operator<(width_t, width_t) = delete;
bool operator<=(width_t, width_t) = delete;
bool operator>=(width_t, width_t) = delete;
bool operator>(width_t, width_t) = delete;

} // namespace dep0::ast
