# Motivation

I started this project as a challenge. I discovered dependent types when learning Idris
and after some time I started thinking that if I had to write a programming language like
C++ it would certainly be a big project for me but, given enough time, I might be able to do it;
but if I had to write a language like Idris I would not even know where to start.
So I started studying Type Theory with the only intention to learn something.
But then what was initially unimaginable became actually quite feasible.

DepC is therefore a challenge and an experiment: what if C/C++ had dependent types?

It probably helps to also show what dependent types are.
Some people are already familiar with dependent types but many aren't.
A more detailed explanation is given in [01 - A Super Quick Primer on Type Theory](01_type_theory.md)
but an even quicker intuition goes like this.
Take the C++ standard library function `std::find()`.
It takes the 2 iterators delimiting the begin and end of a range and it looks for
the first occurrence of some input value. Very simple to understand and equally simple to implement.
It is also very easy to use. At least until you only have one container.
It is possible to imagine that you might have 2 vectors `xs` and `ys` in your current function scope
and that you might accidentally invoke `std::find(xs.begin(), ys.end(), val)`.
It's a very silly mistake to make but there's nothing preventing us from doing it.
The problem here is that, as far as the compiler is concerned,
`xs.begin()` and `ys.end()` have the same type, namely `std::vector<value_type>::const_iterator`,
and they can be compared for equality. It's only in the documentation from the C++ Standard Library
that this operation is disallowed. But if C++ had dependent types, the type of `xs.begin()` could
hypothetically be some `std::const_iterator<xs>` and, similarly, `ys.end()` could have type
`std::const_iterator<ys>`. Now their types, being "templated" against `xs` and `ys`, are different
and the equality operator would not even be defined, thereby lifting a runtime error at compile-time.

A more sophisticated example of dependent types uses the so-called Curry-Howard isomorphism.
Again, many people know about this but many don't. What it means is that if you have a powerful 
enough programming language it can be used not only for ordinary computer programs but it can
also encode logical statements and their proofs, including statements about the program itself.
For example, if you have a function invocation `sort(xs)` that is supposed to sort the vector `xs`,
you can write the dependent type`sorted_t(sort(xs))` that tells the compiler that the result of
invoking `sort(xs)` is, in fact, a sorted vector; if it isn't it's a compile-time error.
