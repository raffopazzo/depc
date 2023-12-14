# A Super Quick Primer on Type Theory

The theory behind DepC is called Calculus of Construction.
It is part of the so-called
[Lambda Cube](https://en.wikipedia.org/wiki/Lambda_cube).
This is a generalization of the so-called Simply-Typed Lambda Calculus,
in which types and terms can both depend on types and terms.

Here I will cover the very basics and very quickly, in particular:
* [Simply-Typed Lambda Calculus](#simply-typed-lambda-calculus)
  * [Alpha-Conversion and Alpha-Equivalence](#alpha-conversion-and-alpha-equivalence)
  * [Substitution, Beta-Reduction and Beta-Equivalence](#substitution-beta-reduction-and-beta-equivalence)
  * [Strong Normalization Theorem](#strong-normalization-theorem)
  * [Context and Typing rules](#context-and-typing-rules)
* [Polimorphic Functions](#polymorphic-functions)
* [Kinds](#kinds)
* [Dependent Types](#dependent-types)
* [Everything Together](#everything-together)
* [Definitions and Delta-Reduction](#definitions-and-delta-reduction)

## Simply-Typed Lambda Calculus

This is the starting point.
C and other "simple" languages operate pretty much in this space.
Here you start by having 2 infinite sets:
* an infinite set `V` of variable names
* an infinite set `T` of type names.

Types come in 2 forms and can be constructed recursively:
* a simple type, say `t` or `int`, whose name is taken from the set `T`;
* a function type (aka *arrow type*) of the form `<type> -> <type>`,
  for example `int -> string`.

In order to make the theory simpler to develop/expose, functions are curried.
This means that functions taking N arguments are modeled as functions
that take 1 argument and return a function taking N-1 argumets.
Therefore, the type of a function to add 2 integers is `int -> int -> int`.
However, this is only to simplify the theory.
Everything works just as fine in uncurried languages, such as DepC.
All you have to do is take care of all the details correctly.
In DepC, the type of a function to add 2 integers is `(int, int) -> int`.

Expressions come in 3 forms and can be constructed recursively:
* a variable, say `x`, whose name is taken from the set `V`;
* a function application `<expr> <expr>`, say `negate b`;
* a function abstraction `lambda x:<type> . <expr>`,
  for example `lambda a:int . lambda b:int . a + b`,
  or `lambda f:int->int . f 0`;

### Alpha-Conversion and Alpha-Equivalence

This is the concept that 2 expressions "mean" the same thing,
if they only differ in the name of function arguments.
For example, in C++ `[] (int x) { return x; }` is the same as
`[] (int y) { return y; }` even though `x` and `y` have different names.
Note that, instead, `[] (int x) { return x + y; }` and
`[] (int y) { return y + y; }` mean, quite obviously, different things.
So these two are not alpha-equivalent.
Neither are `[] (int x) { return x; }` and `[] (double x) { return x; }`.
Alpha-conversion is simply the process of renaming an expression
into another that is alpha-equivalent to it.

### Substitution, Beta-Reduction and Beta-Equivalence

Substitution (more precisely *capture-avoiding substitution*)
is written `M[a:=B]` and is the process of replacing
all occurrences of variable `a` inside expression `M` with expression `B`.
This operation is not completely trivial, because one needs to make sure that,
the meaning of the resulting expression has not been changed accidentally.
In particular, it needs to avoid (accidental) captures, hence the name.
For example, if `M` is `lambda x:int . lambda y:int . x + y`,
and `B` is simply `y`, the substitution `M[x:=y]` **is not**
`lambda y:int . y + y` but, instead, `lambda w:int . y + w`,
or some other expression alpha-equivalent to it.

An application whose left component is a lambda abstraction,
can be reduced by substituting the right component
everywhere the function argument appears in the body of the abstraction.
For example, in C++ `[] (int x) { return x + x; }(2)` can be reduced
(by so-called one-step beta-reduction) to `2 + 2`.
If you continue reducing until you no longer have expressions of the form
`(lambda x:T . M)(N)`, then you have obtained a so-called normal-form.
For example, `2 + 2` can be further reduced to `4`,
by means of some primitive knowledge of the expression `+`.

Because expressions are recursive, it is possible to have a choice
about what sub-expression to beta-reduce first.
Whenever this is the case, a so-called *Convergence Theorem* states that,
regardless of the choices you make, you will eventually converge to
a unique beta-normal form.

Finally, if two expressions eventually reduce to the same beta-normal form,
they are defined to be beta-equivalent.

### Strong Normalization Theorem

In Simply-Typed Lambda Calculus, beta-normalization
of any arbitrary expression will always terminate.
This is not the case in other settings,
for example in Untyped Lambda Calculus,
or in the presence of so-called infinite types.

### Context and Typing rules

A context `C` is like a function scope in a programming language.
It maps variable names to their types.

Typing rules are expressed using the syntax of inference rules from logic:
requirements (aka *premises*) are shown above a line;
the conclusion is shown below the line.
Any time the premisises are satisfied, the conclusion can be drawn.
Both premises and conclusions are expressed as so-called *judgements*,
which have the form `C |- M:t`, meaning:
context `C` entails that expression `M` has type `t`.

Judgements can always be written down and they have no particular meaning.
Only judgements that can be verified according to some typing rules
are important and they result in a so-called *legal term*,
i.e. a term that has been assigned a valid type in some valid context.

Below are the typing rules in Simply-Typed Lambda Calculus,
and this is their interpretation in plain english:
* the Var rule is the base case of the recursion:
  if a variable appears in your context,
  then it has the type specified by that context;
* the App rule says that the type of a function application
  is the return type of the function being invoked,
  as long as the argument to which it is applied has the correct type;
* the Abs rule says that, if you have a context `C`
  extended by one more declaration `x:a`,
  and it entails that some expression `M` has type `b`,
  then you can drop the `x` from your context
  and introduce a new function of type `a -> b`.

```
Var:
   x:a in C
   --------
   C |- x:a

App:
   C |- M: a->b
   C |- N:a
   ------------
   C |- M N: b

Abs:
   C, x:a |- M: b
   ----------------------------
   C |- lambda x:a . M : a -> b
```

### Type-Assignment, Type-Checking and Term-Finding

There are 3 important questions that one can ask:
* Type-Assignment: if I have a context `C` and an expression `M`,
  what is the type of `M`? i.e. `C |- M:?`
* Type-Checking: if I have a context `C`, expression `M` and type `t`,
  is it true that `M` has type `t`?
* Term-Finding: if I have a context `C` and a type `t`,
  is there an expression of that type? i.e. `C |- ?:t`

Note that Type-Checking is equivalent to performing Type-Assignment,
and then checking that the assigned type is actually `t`.

In Simply-Typed Lambda Calculus all of the above problems are decidable,
meaning that you can come up with an algorithm to answer these questions.
This is not always the case, at least for term-finding.

## Polymorphic Functions

Here you find "terms depending on types".

In Simply-Typed Lambda Calculus the identity function on integers
is expressed by `lamda x:int . x`.
Similary, the identity function on doubles is `lambda x:double . x`.
You can repeat the process for any type, including function types.
The identity of functions `int -> int` is `lambda x: int -> int . x`.

With Polymorphic Functions you can define an identity function for all types.
You can, in fact, take as first argument the type of `x`,
i.e. `lambda t:* . lambda x:t . x` where `*` is "the type of all types".
In C++ this would be `[] <typename t> (t x) { return x; }`.

But what is the type of `[] <typename t> (t x) { return x; }`?
In C++ it's something made up by the Standard Committee without
any theoretical foundation, but it does actually a valid type.
This type is called a *Pi-type* and is expressed as `Pi t:* . <type>`.
So `lambda t:* . lambda x:t . x` has type `Pi t:* . t -> t`;
this is the type that the C++ expression should have (but it hasn't).

The typing rules of Simply-Typed Lambda Calculus need to be extended
to account for this possibility that you can apply an expression to a type.

```
Var:
   x:a in C
   --------
   C |- x:a

App1 (aka 1st order application):
   C |- M: a->b
   C |- N:a
   ------------
   C |- M N: b

Abs1 (aka 1st order abstraction):
   C, x:a |- M: b
   ----------------------------
   C |- lambda x:a . M : a -> b

App2 (aka 2nd order application):
   C |- M: (Pi a:* . A)
   C |- B:*
   --------------------
   C |- M B : A[a:=B]

Abs2 (akd 2nd order abstraction):
   C, a:* |- M: A
   ---------------------------------
   C |- lambda a:* . M : (Pi a:*. A)
```

## Kinds

Here we start over from Simply-Typed Lambda Calculus
and add "types depedening on types".
Note that there are no Pi-types here, we are back to only arrows.

Imagine something like `lambda a:* . a -> a`.
This is similar to `Pi a:* . a -> a` but,
whilst `Pi` denotes a type, lambda denotes an expression.
So this is an expression that takes a type and returns a type.
It is called a *type constructor*.
So what is its type now?
Similary to arrows in Simply-Typed Lambda Calculus,
its type is `* -> *`, since it takes a type and returns a type.

In C++ this is very similar to `std::vector`.
In fact, `vector` has type `* -> * -> *`,
because it takes 2 template parameters:
one for the element and one for the allocator.
You can also "pass" it to a function,
say `template <template <typename, typename> typename> int f();`,
by calling `f<std::vector>()`.

So now, `int` is a type (i.e. `int : *`)
and `vector : * -> * -> *` is called a *kind*.
Note that there is no value "of type vector",
i.e. there is no expression `M` such that `M: vector`.
There are values of type `vector<int, allocator>`,
but not of type `vector` alone.
Neither in C++ nor in Type Theory.

In the same way that we use `*` to mean any type,
we can also use `[]` (called *box*) to mean any kind,
eg `* : []`, `* -> * : []`, `* -> * -> *: []`, etc.
So, in some sense, "box is the type of all kinds".

If we use `s` to denote either a type or a kind,
we can define the following typing rules.

```
Sort:
   <empty context> |- * : []

Var:
   C |- A: s
   ------------- if x not already defined in C
   C, x:A |- x:A

Weak:
   C |- A:B
   C |- T:s
   ------------- if x not already defined in C
   C, x:T |- A:B

Form:
   C |- A:s
   C |- B:s
   ---------------
   C |- A -> B : s

App:
   C |- M : A -> B
   C |- N : A
   ---------------
   C |- M N : B

Abs:
   C, x: A |- M : B
   C       |- A -> B : s
   ----------------------------
   C |- lambda x:A . M : A -> B

Conv:
   C |- A: B
   C |- B':s
   ---------- if B and B' are beta-equivalent
   C |- A: B'
```

Again, `s` plays a double role here; if you wanted to be explicit,
you would have to double up the number of rules.

Note that in the above rules,
the constraint "if x not already defined in C"
can be dropped if one takes care of shadowing correctly.
This is what is done in all programming languages, including DepC.

The above rules are only slightly more complicated than before,
and a couple of them are only there to fix some technicalities:
* Sort is the base case of the recursion: `*` is always a kind;
* Var is somewhat the same as before, except that now,
  in order to add a variable to a context,
  you also have to make sure that its type is valid;
  for example, `vector<vector, allocator>` is not a valid type;
* Weak is there only to fix a technicality:
  as it stands, the Var rule can only be used if
  the variable `x` was the last one declared;
  but, clearly, adding more variables to the context
  does not affect previously declared variables;
* Form allows to construct arrows, either in types or in kinds,
  eg `int -> int` or `* -> *`;
  note that neither `int -> *` nor `* -> int` are possible in this setting;
* App should be fairly intuitive:
  you can apply `int -> string` to an `int` to obtain a `string`,
  or a `* -> *` to a `*` to obtain a `*`;
* Abs should equally be fairly intuitive:
  it introduces new functions, eg of type either `int -> int` or `* -> *`;
* Conv (the Conversion rule) is a bit of a technicality,
  but it will become more important later;
  essentially, it says that you might have to perform beta-reduction
  in types, because a term of type `int` and a term of type
  `(lambda t:* . t)int` should be understood as the same type.

## Dependent Types

We start again from Simply-Typed Lambda Calculus,
and add "types depending on terms.
Here we reintroduce Pi-types.

In C++ you can do `std::array<int, 3>`.
In some sense, this is already a type that depends on a term, `3` in this case.
But you cannot generalize this to, say, `auto f(int n) -> std::array<int, n>`.

With types depending on terms you can write such an `f`, i.e.
`lambda n:int . array int n`.
So what is its type?
It takes an `int` and returns a type, so you could say its type is `int -> *`.
But in this case, not only it returns a `*`,
but it also uses the value of `n` to compute such type.
So we have to reintroduce Pi-types.
Its type is, in fact, `Pi n:int . array int n`.
Note that now the body of the Pi-type contains
a whole expression, not just an arrow.

The typing rules are very similar to the ones seen for kinds.
The only differences are in the Form, App and Abs rules,
where all arrows have been replaced by Pi-types.
In fact, a Pi-type `Pi x:A . B` where `x` does not occur in `B`,
can be written as `A -> B`.

```
Sort:
   <empty context> |- * : []

Var:
   C |- A: s
   -------------
   C, x:A |- x:A

Weak:
   C |- A:B
   C |- T:s
   -------------
   C, x:T |- A:B

Form:
   C      |- A:*
   C, x:A |- B:s
   -------------------
   C |- Pi x:A . B : s

App:
   C |- M : (Pi x:A . B)
   C |- N : A
   ---------------------
   C |- M N : B[x:=N]

Abs:
   C, x: A |- M : B
   C       |- (Pi x:A . B) : s
   ----------------------------------
   C |- lambda x:A . M : (Pi x:A . B)

Conv:
   C |- A: B
   C |- B':s
   ---------- if B and B' are beta-equivalent
   C |- A: B'
```

## Everything Together

The *Calculus of Construction* is the result of combining
all these extension of Simply-Typed Lambda Calculus.
Here you can have all comibations:
types and terms depending on types and terms.

Astonishingly, all it takes is a slight modification
in just the Form rule, whereby the sort of the two premises
can be chosen independently from one another, say `s1` and `s2`.

```
Sort:
   <empty context> |- * : []

Var:
   C |- A: s
   -------------
   C, x:A |- x:A

Weak:
   C |- A:B
   C |- T:s
   -------------
   C, x:T |- A:B

Form:
   C      |- A:s1
   C, x:A |- B:s2
   --------------------
   C |- Pi x:A . B : s2

App:
   C |- M : (Pi x:A . B)
   C |- N : A
   ---------------------
   C |- M N : B[x:=N]

Abs:
   C, x: A |- M : B
   C       |- (Pi x:A . B) : s
   ----------------------------------
   C |- lambda x:A . M : (Pi x:A . B)

Conv:
   C |- A: B
   C |- B':s
   ---------- if B and B' are beta-equivalent
   C |- A: B'
```

Type-assignment and Type-checking remain both decidable,
so it is possible to construct a computer program to perform type-checking.
Term-finding is, however, undecidable, so it is impossible to construct a
program that will certainly find a term for any given type, only best-effort.
Under the Curry-Howard isomorphism, this means that theorem-proving will
always require human work. Computers can, at best, assist in the task.

Another fundamental difference between Simply-Typed Lambda Calculus,
i.e. the C programming language, and Calculus of Construction, i.e. DepC,
is that there is no distinction between types and expressions.
This is reflected in the AST where only the node for
expressions exists and there is no node for types.

## Definitions and Delta-Reduction

In any programming language you can define a top-level function
and give it a name, say `f`. You can later call that function from
within another one, say from `g`.
With dependent types it also means that you
may call to that function from within a type expression,
eg `lambda x:f(int) . g(x)`.
If the body of the top-level function `f` is visible,
you can expand the expression `f(int)` by substituting `f` for its body.
This operation is called Delta-Reduction.

Delta and Beta reductions can be combined into a Beta-Delta reduction.
Here the reduction proceeds by sequencing beta and delta reduction
steps in any order, either at will or as necessary.

This means that the conversion rule needs to be slightly altered;
instead of the constraint "if B and B' are beta-equivalent" it
should now read "if B and B' are beta-delta-equivalent".
