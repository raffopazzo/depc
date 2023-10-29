add_library(dep0_testing_lib
    include/dep0/testing/ast_predicates.hpp
    include/dep0/testing/failure.hpp
    include/dep0/testing/literal_string.hpp
    include/dep0/testing/predicate.hpp
    include/dep0/testing/pretty_name.hpp
    src/ast_predicates.cpp
    src/failure.cpp
    src/literal_string.cpp
    src/predicate.cpp
    src/pretty_name.cpp
    )
add_library(DepC::Dep0::Testing ALIAS dep0_testing_lib)
target_compile_features(dep0_testing_lib PUBLIC cxx_std_20)
target_include_directories(dep0_testing_lib PUBLIC include)
target_link_libraries(dep0_testing_lib
  PUBLIC
    DepC::Dep0::Ast
    Boost::Boost
    )
