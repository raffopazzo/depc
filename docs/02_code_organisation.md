# Code Organisation, Dep0 and DepC

The source tree is organised as follows:
* `antlr4/` contains lexer and parser grammar and the ANTLR4 jar used during compilation to generate the C++ code that does the parsing of `.depc` source file.
* `dep0/` is the bootstrapping compiler written in C++20 or later.
It is my desire to one day reach self-hosting (i.e. write a DepC compiler in DepC).
That day a new `depc/` folder will contain its source code.
Inside `dep0/` the code is organised in numbered folders so that lexicographical order and dependency order match.
In particular:
  * `app/dep0` contains the `main.cpp` of the final `dep0` compiler, which assembles the pipeline stages in `lib/` together with the LLVM machinery.
  * `lib/00_antlr/` contains the CMake targets to generate the C++ parser from the ANTLR4 grammars mentioned earlier.
  * `lib/00_core/` contains general purpose core functionality like mmap-ing a source file, error handling, etc.
  * `lib/01_ast/` contains the generic (i.e. templated) Abstract Syntax Tree and some functions that operate on it, eg beta-reduction or renaming.
    More details can be found in [03 - Dep0: AST and Pipeline](03_dep0.md).
  * `lib/02_parser/` contains the entry point of the parsing stage of the compiler pipeline.
  * `lib/03_typecheck/` contains the entry point of the typechecking stage.
  * `lib/04_transform/` contains the entry point of the transformation stage along with some transformations that can be applied on a legal AST (i.e. an AST that typechecks correctly)
  * `lib/04_llvmgen/` contains the entry point of the LLVM IR code-gen stage.
  * `lib/99_testing/` contains general purpose testing facilities, among all
  [custom Boost.Testing predicates](https://beta.boost.org/doc/libs/1_82_0/libs/test/doc/html/boost_test/testing_tools/custom_predicates.html)
  for the templated AST.
* `testfiles/` contains source code in DepC used for testing the compiler, again currently `dep0` but hopefully one day `depc` too. Within it, tests are organised in numbered sections, so that if people are working on different features at the same time they can each reserve a section for their feature and put the test files within that, so to minimise PR conflicts. Within each numbered section test files can be named pretty much at will but, unless there's a strong reason to diverge, the recommended approach is to have numbered test files per category among: `pass` (meaning this code should compile normally), `parser_error` (meaning that the compiler is supposed to detect a parsing error, please add a comment to highlight where the error is) and `typecheck_error` (meaning that the compiler should detect a typechecking error, again please add a comment to highlight where the error is). For instance `0000_basics/pass_000.depc` or `0006_kinds/parser_error_002.depc`.
* `tools/` contains a potentially assorted array of tools; currently only `devbox` which is a docker image with the toolchain for (hopefully) reproducible builds.
