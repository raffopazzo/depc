add_subdirectory(test)
add_library(dep0_parser_lib
  include/dep0/parser/ast.hpp
  include/dep0/parser/parse.hpp
  src/ast.cpp
  src/parse.cpp
  )
add_library(DepC::Dep0::Parser ALIAS dep0_parser_lib)
target_compile_features(dep0_parser_lib PUBLIC cxx_std_20)
target_include_directories(dep0_parser_lib PUBLIC include)
target_link_libraries(dep0_parser_lib
  PUBLIC
    DepC::Dep0::Ast
  PRIVATE
    Boost::Boost
    DepC::Dep0::Antlr4
    )
