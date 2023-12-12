Everybody loves good documentation, but good documentation is hard because hard is striking a balance between:
* the level of details of the documentation,
* the time it takes to write it and
* the time it takes for new changes to make it outdated.

Therefore here in `docs/` I will only focus on fundamental concepts that are unlikely to change throughout the development of this project.
Anything more detailed than that will be documented directly in code, so to minimise the chances of stale documentation.

This documentation is organised in sections.
Sections are numbered so that lexicographical order and dependency order match. Here's an overview of all the sections:

* [00 - Motivation and Goal](00_intro.md) covers why I started doing this and where I'm trying to go with it.
* [01 - A Super Quick Primer on Type Theory](01_type_theory.md) summarises the primary concepts from Type Theory that play an important (or even fundamental) role in the development of this project. Some people are already familiar with these concepts and some aren't. For those who aren't this is hopefully enough to see where certain parts of the source code (eg alpha and beta equivalences) are coming from and to perhaps inspire them to pick up a proper book on Type Theory; I suggest to follow the list below. For those who are already familiar, it will only take a few minutes to go over it and it will probably help clarify the exact setting in which this language operates.
* [02 - Code Organisation, Dep0 and DepC](02_code_organisation.md) explains how I am structuring the source tree. It's a very simple, non-mind-blowing read.
* [03 - Dep0: AST and Pipeline](03_dep0.md) explains the fundamental concepts of the internal design of Dep0. As mentioned above, anything more detailed than this is hopefully documented directly in code. If not, feel free to raise a PR.

## Suggested Reading

* [Type Theory and Formal Proof: An Introduction](https://amzn.eu/d/8dl6Ktx) this is **the most important** book; if you read nothing else at least try read the first 6 chapters of this.
* [Types and Programming Languages](https://amzn.eu/d/iBVHn9a) is a famous textbook in this field and you might come across many papers and publications that reference it.
* [Interactive Theorem Proving and Program Development: Coq’Art](https://amzn.eu/d/2syZqOi) is a bit of a more advanced reading; it covers advanced topics like the Calculus of Inductive Construction whilst also showing the famous theorem prover _Coq_. It is also referenced by many papers and publications, including the above 2 books.
