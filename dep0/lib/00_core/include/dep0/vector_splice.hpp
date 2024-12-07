/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header defining `dep0::vector_splice()`.
 */
#pragma once

#include <iterator>
#include <vector>

namespace dep0 {

/**
 * @brief Insert new elements in the given vector at exactly the given position.
 *
 * If `pos` is the end iterator, this behaves exactly like `std::vector::insert`.
 * If `elements` is empty, this behaves exactly like `std::vector::erase(pos)`,
 * except that passing the end iterator is fine.
 * Otherwise, the element pointed by `pos` is replaced by the first of `elements`,
 * and all other new elements are inserted after it.
 *
 * @return  The iterator pointing at the beginning of where the new elements have been inserted.
 *          If the input range of elements was empty, the iterator past `pos` is returned instead,
 *          unless it already was the end iterator, in which case the end iterator is returned and
 *          this call does nothing.
 */
template <typename T>
typename std::vector<T>::iterator
vector_splice(
    std::vector<T>& v,
    typename std::vector<T>::iterator const pos,
    std::vector<T> elements)
{
    if (elements.empty())
        return pos == v.end() ? pos : v.erase(pos);
    else if (pos == v.end())
        return v.insert(pos, std::make_move_iterator(elements.begin()), std::make_move_iterator(elements.end()));
    else
    {
        // insert might invalidate `pos`, so have to know the exact location in order to compute the result iterator
        auto const n = std::distance(v.begin(), pos);
        auto it = elements.begin();
        *pos = std::move(*it++);
        v.insert(std::next(pos), std::make_move_iterator(it), std::make_move_iterator(elements.end()));
        return std::next(v.begin(), n);
    }
}

} // namespace dep0
