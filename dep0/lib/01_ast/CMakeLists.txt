# add_subdirectory(test)
add_library(dep0_ast_lib
    include/dep0/ast/ast.hpp
    include/dep0/ast/concepts.hpp
    include/dep0/ast/pretty_print.hpp
    include/dep0/ast/traverse.hpp
    src/ast.cpp
    src/concepts.cpp
    src/pretty_print.cpp
    src/traverse.cpp
    )
add_library(DepC::Dep0::Ast ALIAS dep0_ast_lib)
target_compile_features(dep0_ast_lib PUBLIC cxx_std_20)
target_include_directories(dep0_ast_lib PUBLIC include)
target_link_libraries(dep0_ast_lib
  PUBLIC
    DepC::Dep0::Core
    Boost::Boost
    )
