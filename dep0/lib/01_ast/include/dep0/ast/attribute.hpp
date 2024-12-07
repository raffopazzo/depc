/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::ast::attribute_t`.
 */
#pragma once

#include "dep0/source.hpp"

namespace dep0::ast {

/**
 * @brief Represents a single attribute, eg `[[builtin]]`.
 *
 * Currently an attribute can only be a single string but
 * in future could be extended to expressions eg `[[assume(x > 0)]]`.
 */
struct attribute_t
{
    source_text value;
};

} // namespace dep0::ast
