add_subdirectory(test)
add_library(dep0_core_lib
    include/dep0/digit_separator.hpp
    include/dep0/error.hpp
    include/dep0/mmap.hpp
    include/dep0/source.hpp
    include/dep0/unique_ref.hpp
    src/digit_separator.cpp
    src/error.cpp
    src/mmap.cpp
    src/source.cpp
    src/unique_ref.cpp
    )
add_library(DepC::Dep0::Core ALIAS dep0_core_lib)
target_compile_features(dep0_core_lib PUBLIC cxx_std_20)
target_include_directories(dep0_core_lib PUBLIC include)
target_link_libraries(dep0_core_lib PRIVATE Boost::Boost)
