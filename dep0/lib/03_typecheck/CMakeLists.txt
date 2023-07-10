add_subdirectory(test)
add_library(dep0_typecheck_lib
  include/dep0/typecheck/ast.hpp
  include/dep0/typecheck/check.hpp
  src/ast.cpp
  src/check.cpp
  )
add_library(DepC::Dep0::TypeCheck ALIAS dep0_typecheck_lib)
target_compile_features(dep0_typecheck_lib PUBLIC cxx_std_20)
target_include_directories(dep0_typecheck_lib PUBLIC include)
target_link_libraries(dep0_typecheck_lib PUBLIC DepC::Dep0::Ast DepC::Dep0::Parser)
