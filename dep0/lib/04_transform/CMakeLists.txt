add_subdirectory(test)
add_library(dep0_transform_lib
  # include/dep0/transform/recipes/default_llvmgen.hpp
  # src/recipes/default_llvmgen.cpp
  include/dep0/transform/run.hpp
  include/dep0/transform/transform.hpp
  src/run.cpp
  src/transform.cpp
  )
add_library(DepC::Dep0::Transform ALIAS dep0_transform_lib)
target_compile_features(dep0_transform_lib PUBLIC cxx_std_20)
target_include_directories(dep0_transform_lib PUBLIC include)
target_link_libraries(dep0_transform_lib PUBLIC DepC::Dep0::TypeCheck)

