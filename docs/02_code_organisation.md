# Code Organisation, Dep0 and DepC

The source tree is organised as follows:
* `antlr4/` contains lexer and parser grammar and the ANTLR4 jar used during compilation to generate the C++ code that does the parsing of `.depc` source file.
* `dep0/` is the bootstrapping compiler written in C++20 or later. It is my desire to one day reach self-hosting (i.e. write a DepC compiler in DepC). That day a new `depc/` folder will contain its source code.
* `testfiles/` contains source code in DepC used for testing the compiler, again currently `dep0` but hopefully one day `depc` too. Within it, tests are organised in numbered sections, so that if people are working on different features at the same time they can each reserve a section for their feature and put the test files within that, so to minimise PR conflicts. Within each numbered section test files can be named pretty much at will but, unless there's a strong reason to diverge, the recommended approach is to have numbered test files per category among: `pass`, `parse_error` and `typecheck_error`. For instance `0000_basics/000_pass.depc` or `0006_kinds/002_typecheck_error.depc`
