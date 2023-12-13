# A Super Quick Primer on Type Theory

The theory behind DepC is called Calculus of Construction and is part of the so-called [Lambda Cube](https://en.wikipedia.org/wiki/Lambda_cube).
This is a generalization of the so-called Simply-Typed Lambda Calculus in which types and terms can both depend on types and terms.

Here I will cover the very basics and very quickly, in particular:
* Simply-Typed Lambda Calculus
  * Alpha-Conversion and Alpha-Equivalence
  * Beta-Reduction and Beta-Equivalence
  * Strong Normalization Theorem
  * Context and Typing rules
* Polimorphic Functions
* Kinds
* Dependent Types
* Everything together

## Simply-Typed Lambda Calculus

This is the starting point. C and other "simple" languages like it operate pretty much in this space.
Here you start by having 2 infinite sets:
* an infinite set `V` of variable names
* an infinite set `T` of type names.

Types come in 2 forms and can be constructed recursively:
* a simple type, say `t` or `int`, whose name is taken from the set `T`;
* a function type (also called *arrow type*) of the form `<type> -> <type>`, for example `int -> short`.

In order to make the theory simpler to develop, everybody uses curried functions,
so functions taking N arguments are modeled as functions that take 1 argument and return a function taking N-1 argumets.
Therefore the type of a function that adds 2 integers together has type `int -> int -> int`.

Expressions come in 3 form and can be constructed recursively:
* a variable, say `x`, whose name is taken from the set `V`;
* a function application `<expr> <expr>`, say `negate b`;
* a function abstraction `lambda x:<type> . <expr>`, for example `lambda a:int . lambda b:int . a + b` or `lambda f:int->int . f 0`;

### Alpha-Conversion and Alpha-Equivalence

This is the concept that 2 expressions "mean" the same thing if they only differ in the name of function arguments.
For example in C++ `[] (int x) { return x; }` is the same as `[] (int y) { return y; }` even though `x` and `y` have different names.
Note that instead `[] (int x) { return x + y; }` and `[] (int y) { return y + y; }` mean, quite obviously, different things.
So these two are not alpha-equivalent, neither are `[] (int x) { return x; }` and `[] (double x) { return x; }`.
Alpha-conversion is simply the process of renaming an expression into another that is alpha-equivalent to the initial one.

### Substitution, Beta-Reduction and Beta-Equivalence

Substitution, written `M[a:=B]` is the process of replacing all occurrences of variable `a` inside the expression `M` with expression `B`.
This operation is not completely trivial as one needs to make sure that when performing this substituion the meaning of the resulting
expression has not been accidentally changed. For example, if `M` is `lambda x: int . lambda y: int . x + y` and `B` is simply `y`,
the substitution `M[x:=y]` **is not** `lambda y: int . y + y` but instead `lambda w: . y + w` or some other alpha-equivalent expression.

An application expression, whose left component is a lambda abstraction, can be reduced by substituting the right component
everywhere the function argument appears inside the body of the abstraction.
For example, in C++ `[] (int x) { return x + x; }(2)` can be reduced (by so-called one-step beta-reduction) to `2 + 2`.
If you continue reducing until you no longer have any expressions of the form `(lambda x:<type> . <expr>)(<expr>)`
then you have obtained a so-called term in normal-form. For example, `2 + 2` can be further reduced to `4`,
by means of some primitive knowledge of the expression `+`.

Because expressions are defined recursively, it is possible that you might have a choice about what sub-expression to beta-reduce first.
Whenever this is the case, a so-called *Convergence Theorem* states that regardless of the choice you make you will eventually
converge to a unique beta-normal-form.

Finally, if two expressions eventually reduce to the same beta-normal-form, they are defined as beta-equivalent.

### Strong Normalization Theorem

In Simply-Typed Lambda Calculus, beta-normalization always eventually terminate for any arbitrary expression.
This is not the case in other settings, for example in Untyped Lambda Calculus.

### Context and Typing rules

A context `C` is the equivalent of a function scope in programming languages. It binds variable names to their types.

Typing rules are expressed using the same syntax as inference rules from logic:
requirements (aka *premises*) are shown above a line; conclusions are shown below the line.
Any time the premisises are satisfied, the corresponding conclusion can be drawn.
Both premises and conclusions are expressed as so-called *judgements* which have the form
`C |- M:t` meaning that the context `C` entails that expression `M` has type `t`.

The following are the typing rules in Simply-Typed Lambda Calculus:
* the Var rule is the base case: if a variable appears in your context, then it has the type specified by that context;
* the App rule says that the type of a function application is the return type of the function, as long as the argument has the correct type;
* the Abs rule says that if you have a context `C` extended by one more declaration `x:a`, and it entails that some expression `M` has type `b`,
then you can drop the `x` from your context and introduce a new function abstraction of type `a -> b`.

```
Var:
   x:a in C
   --------
   C |- x:a

App:
   C |- M: a->b   and   C |- N:a
   -----------------------------
   C |- M N: b

Abs:
   C, x:a |- M: b
   ----------------------------
   C |- lambda x:a . M : a -> b
```

### Type-Assignment, Type-Checking and Term-Finding

There are 3 important questions that one can ask:
* Type-Assignment: if I have a context `C` and an expression `M`, what is the type of `M`? i.e. `C |- M:?`
* Type-Checking: if I have a context `C`, expression `M` and type `t`, is it true that `M` has type `t`?
* Term-Finding: if I have a context `C` and a type `t`, is there an expression of that type? i.e. `C |- ?:t`

Note that Type-Checking is equivalent to performing Type-Assignment and then checking that the assigned type is actually `t`.

In Simply-Typed Lambda Calculus all of the above problems are decidable, meaning you can come up with an algorithm to answer these questions.

## Polymorphic Functions

Here you find "terms depending on types".

In Simply-Typed Lambda Calculus the identity function on integers is expressed by `lamda x:int . x`.
Similary the identity function on doubles is expressed by `lambda x:double . x`.
You can repeat the process for any type, including function types.
The identity function of functions `int -> int` is expressed by `lambda x: int -> int . x`.

With Polymorphic Functions you can define an identity function for all types.
You can, in fact, take as first argument the type of `x`, i.e. `lambda t:* . lambda x:t . x` where `*` indicates any type.
In C++ this would be `[] <typename t> (t x) { return x; }`.

But what is the type of `[] <typename t> (t x) { return x; }`? In C++ it's something made up by the Standard Committee without
any theoretical foundation but it does actually a valid type, called a *Pi-type* and expressed as `Pi t:* . <type>`.
So `lambda t:* . lambda x:t . x` has type `Pi t:* . t -> t`; this is the type that the C++ expression should have (but it hasn't).

The typing rules need to be extended to account for this possibility that you can apply an expression to a type.

```
Var:
   x:a in C
   --------
   C |- x:a

App1 (aka 1st order application):
   C |- M: a->b   and   C |- N:a
   -----------------------------
   C |- M N: b

Abs1 (aka 1st order abstraction):
   C, x:a |- M: b
   ----------------------------
   C |- lambda x:a . M : a -> b

App2 (aka 2nd order application):
   C |- M: (Pi a:* . A)   and   C |- B:*
   -------------------------------------
   C |- M B : A[a:=B]

Abs2 (akd 2nd order abstraction):
   C, a:* |- M: A
   ---------------------------------
   C |- lambda a:* . M : (Pi a:*. A)
```
