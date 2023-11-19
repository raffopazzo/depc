add_subdirectory(test)

add_library(dep0_typecheck_lib
  # public headers
  include/dep0/typecheck/ast.hpp
  include/dep0/typecheck/check.hpp
  include/dep0/typecheck/context.hpp
  include/dep0/typecheck/derivation.hpp
  include/dep0/typecheck/error.hpp
  # private headers
  src/private/beta_equivalence.hpp
  src/private/derivation_rules.hpp
  src/private/returns_from_all_branches.hpp
  # source files
  src/ast.cpp
  src/beta_equivalence.cpp
  src/check.cpp
  src/context.cpp
  src/derivation.cpp
  src/derivation_rules.cpp
  src/error.cpp
  src/returns_from_all_branches.cpp
  )
add_library(DepC::Dep0::TypeCheck ALIAS dep0_typecheck_lib)
target_compile_features(dep0_typecheck_lib PUBLIC cxx_std_20)
target_include_directories(dep0_typecheck_lib PUBLIC include)
target_link_libraries(dep0_typecheck_lib
  PUBLIC
    DepC::Dep0::Ast
    DepC::Dep0::Parser
  )
