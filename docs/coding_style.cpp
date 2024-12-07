#include "library_99/this/file.hpp"

#include "private/bar.hpp"
#include "private/baz.hpp"
#include "private/foo.hpp"

#include "library_98/bar.hpp"
#include "library_98/baz.hpp"
#include "library_98/foo.hpp"

#include <antlr4-runtime/antlr4-runtime.h>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <algorithm>
#include <cmath>
#include <optional>

struct object_t
{
    explicit object_t(int);
    /*implicit*/ object_t(double);
    object_t(int, int);
};

int take_n_sum(std::vector<int> const& xs, std::size_t const k)
{
    // an if-else where both bodies are single-statemnt,
    // is a very good candidate for a ternary operator,
    return k > xs.size()
        ? -1
        : std::accumulate(xs.begin(), xs.begin() + k, 0);
}

int pattern_matching(colour_t const c)
{
    // there's nothing wrong with nested ternary operator,
    // if you are essentially doing pattern matching;
    // just don't over-do it and use your common sense
    using enum colour_t;
    return
        c == red ? 1
        : c == blue ? 10
        : c == green ? 100
        : -1;
}

int zip_sum(std::vector<int> const& xs, std::vector<int> const& ys)
{
    auto const n = std::min(xs.size(), ys.size());
    // in this case, 3 lines of "imperative" code
    // read a lot better than its functional equivalent;
    // in Haskell it would read way better, but not in C++ 
    int sum = 0;
    for (auto const i: std::views::iota(0ul, n))
      sum += xs[i] + ys[i];
    return sum;
}

// in this example `from` is marked `const`, even though
// it is later assigned to `next` which is mutated;
// so why not just leave `from` mutable?
// In general you can, depending on your specific situation;
// in this example it makes it clear that mutation
// only occurs inside the generator.
std::vector<int> make_staircase(int const from, int const n)
{
    std::vector<int> result;
    std::generate_n( // from, from, from+1, from+1, from+2, from+2, ...
        std::back_inserter(result),
        n,
        [next=from, step=0] () mutable
        {
            return std::exchange(next, next + std::exchange(step, step == 1 ? 0 : 1));
        });
    return result;
}

int complicated_if()
{
    if (
        this is a very long...
        and complicated condition...
        that cannot be broken up...
        so use braces even for single-statement bodies...
    )
    {
        return 0;
    }
}

object_t use_complicated_object(int const flavour)
{
    auto const obj = [&]
    {
        object_t obj;
        obj.set_this();
        obj.set_that();
        if (flavour < 0)
            obj.set_negative();
        return obj;
    }();
    // from here onwards obj is no longer modified
    ...
}
