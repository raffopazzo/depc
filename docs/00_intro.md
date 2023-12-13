# Motivation and Goal

I started this project as a challenge.
I discovered dependent types when learning Idris.
After some time I started thinking:

> if I had to write a programming language like C++,
> it would certainly be a big project for me;
> but, given enough time, I might be able to do it.
> But if I had to write a language like Idris,
> I would not even know where to start!

So I bought [Type Theory and Formal Proof: An Introduction](https://amzn.eu/d/8dl6Ktx)
with the only intention to learn something.
But then, what was initially unimaginable became actually quite feasible.

DepC is, therefore, both a challenge and an experiment:
what if C/C++ had dependent types?

The goal of this project is a bit open-ended.
It would, obviously, be great if I could get to a
fully-working, industry-ready programming language
as efficient as C/C++ and as safe as Idris.
But that's a big project, and I don't know how far I can take it.
I will try to take it as far as possible.
If people start contributing it will be easier to take it further.

## What are Dependent Types?

Some people are already familiar with dependent types, but many aren't.
So, it probably helps to also show what dependent types are.
A more detailed explanation is given in
[01 - A Super Quick Primer on Type Theory](01_type_theory.md),
but an even quicker intuition goes like this.
Take the C++ standard library function `std::find()`.
It takes the 2 iterators delimiting the begin and end of a range,
and it looks for the first occurrence of some input value.
Very simple to understand and equally simple to implement.
It is also very easy to use. At least until you only have one container.
In fact, it is possible to imagine that you might have 2 vectors,
`xs` and `ys`, in your current function scope and you might accidentally
invoke `std::find(xs.begin(), ys.end(), val)`.
It is a very silly mistake to make, but there's nothing to prevent it.
The problem is that, as far as the compiler is concerned,
`xs.begin()` and `ys.end()` have the same type,
namely `std::vector<value_type>::const_iterator`.
As such they can be compared for equality.
It's only in the documentation from the C++ Standard Library
that this operation is disallowed.
But, if C++ had dependent types, the type of `xs.begin()` could,
hypothetically, be some `std::const_iterator<xs>` and,
similarly, `ys.end()` could have type `std::const_iterator<ys>`.
Now their types, being "templated" against `xs` and `ys`,
are actually different and the equality operator would not even be defined.
This would lift a runtime error to a compile-time one.

A more sophisticated example uses the so-called Curry-Howard isomorphism.
Again, many people know about this but many don't.
What it means is that, if you have a powerful enough programming language,
it can be used not only for ordinary computer programs,
but also to encode logical/mathematical statements and their proofs;
including statements and proofs about the program itself.
For example, if you have an expression `sort(xs)` to sort the vector `xs`,
you can write the dependent type`sorted_t(sort(xs))`
stating that this call should, in fact, result in a sorted vector;
if it doesn't, it's a compile-time error.
