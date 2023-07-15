# add_subdirectory(test)

add_library(dep0_typecheck_tt_lib
  include/dep0/typecheck/tt/context.hpp
  include/dep0/typecheck/tt/derivation.hpp
  include/dep0/typecheck/tt/term.hpp
  include/dep0/typecheck/tt/type.hpp
  src/context.cpp
  src/derivation.cpp
  src/term.cpp
  src/type.cpp
  )

add_library(DepC::Dep0::TypeCheck::TT ALIAS dep0_typecheck_tt_lib)
target_compile_features(dep0_typecheck_tt_lib PUBLIC cxx_std_20)
target_include_directories(dep0_typecheck_tt_lib PUBLIC include)
target_link_libraries(dep0_typecheck_tt_lib PUBLIC Boost::Boost DepC::Dep0::Core)
