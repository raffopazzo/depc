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
