#pragma once

namespace dep0::ast {

/** Strongly typed boolean to track whether functions, variables and references are mutable or not. */
enum class is_mutable_t
{
    no,
    yes
};

bool operator<(is_mutable_t, is_mutable_t) = delete;
bool operator<=(is_mutable_t, is_mutable_t) = delete;
bool operator>=(is_mutable_t, is_mutable_t) = delete;
bool operator>(is_mutable_t, is_mutable_t) = delete;

} // namespace dep0::ast
