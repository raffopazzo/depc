#pragma once

namespace dep0::ast {

/** Represents the keywords `signed` or `unsigned` used inside a user-defined integral type definition. */
enum class sign_t
{
    signed_v,
    unsigned_v
};

bool operator<(sign_t, sign_t) = delete;
bool operator<=(sign_t, sign_t) = delete;
bool operator>=(sign_t, sign_t) = delete;
bool operator>(sign_t, sign_t) = delete;

} // namespace dep0::ast
