# add_subdirectory(test)
add_library(dep0_ast_lib
    include/dep0/ast/ast.hpp
    include/dep0/ast/builder.hpp
    include/dep0/ast/concepts.hpp
    include/dep0/ast/no_properties_builder.hpp
    include/dep0/ast/pretty_print.hpp
    include/dep0/ast/tree.hpp
    src/ast.cpp
    src/builder.cpp
    src/concepts.cpp
    src/no_properties_builder.cpp
    src/pretty_print.cpp
    src/tree.cpp
    )
add_library(DepC::Dep0::Ast ALIAS dep0_ast_lib)
target_compile_features(dep0_ast_lib PUBLIC cxx_std_20)
target_include_directories(dep0_ast_lib PUBLIC include)
target_link_libraries(dep0_ast_lib
  PUBLIC
    DepC::Dep0::Core
  PRIVATE
    Boost::Boost
    )
