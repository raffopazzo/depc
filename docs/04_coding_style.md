# Coding Style

Coding Style in the repo is not super strict.
The bottom line is readability.
Sometimes a dense one-liner is more readable than a multi-line body;
sometimes a multi-line body is more readable than a dense one-liner.
Be kind to the next person after you and self-review your own code.

That being said, you can follow guidelines below.

## C++ Code

These are the guidelines for C++ code.
Below some made-up code showing an example.

* Header order:
  put `#include` in order of specificity,
  from more specific to more general,
  then alphabetical within the same specificity:
  * first the `hpp` matching the current `cpp`;
  * then private headers from the current library, alphabetically;
  * then public headers from the current library, alphabetically;
  * then public headers of the next library up, alphabetically;
  * then the next library before that, etc;
  * then public headers of 3d party libraries,
    eg antlr, in angle brackets;
  * then boost, again in angle brackets;
  * then standard C++ libraries;
  * then standard libraries,
    but only if a c++ version doesn't exist,
    i.e. prefer `<cmath>` over `<math.h>`.
* East-Const: always `type_t const&`; never `const type_t&`;
* Braces and single-statemnt bodies:
  put curly braces on a separate,
  but leave single-statement bodies without them.
  Except if you need to separate that statement from, say,
  a complex multi-line `if` condition;
  but, even better, try think harder and break down that
  complex condition using helper functions.
* Brace-initializer vs normal parenthesis:
  when constructing an object using its constructor,
  prefer `object(a,b,c)` over `object{a,b,c}`,
  because this makes it clear that a constructor exists.
* Explicit constructors:
  always mark single-argument constructors `explicit`,
  unless you actually intended to have implicit conversion,
  in which case write `/*implicit*/ object(arg)`
* Always Auto Const:
  always use `auto const` (or `type const`),
  including for function arguments,
  unless you actually intend to mutate things.
  This also applies to the index of `for` loops,
  in which case use `std::views::iota`,
  i.e. `for (auto const i: std::views::iota(...))`,
  because this makes your intentions more explicit,
  and it's also obvious that you are not skipping iterations,
  or otherwise messing with the index.
* Functional vs imperative:
  do use functional style but don't go over-the-top;
  at the end of the day, this is C++ not Haskell;
  sometimes imperative reads better than functional,
  even though it would read very well in Haskell;
  that's because in C++ the functional style came late.
* Use STL algorithms:
  if you can't find a way, try harder;
  if you still can't, then it's fine,
  as long as you don't just give up right away.

```cpp
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
    if (k > xs.size())
        return -1;
    return std::accumulate(xs.begin(), xs.begin() + k);
}

int zip_sum(std::vector<int> const& xs, std::vector<int> const& ys)
{
    auto const n = std::min(xs.size(), ys.size());
    int sum = 0;
    for (auto const i: std::views::iota(0ul, n))
      sum += xs[i] + ys[i];
    return sum;
}

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
```
