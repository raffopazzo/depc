add_subdirectory(test)
add_library(dep0_core_lib
    include/dep0/destructive_self_assign.hpp
    include/dep0/digit_separator.hpp
    include/dep0/error.hpp
    include/dep0/fmap.hpp
    include/dep0/match.hpp
    include/dep0/mmap.hpp
    include/dep0/scope_map.hpp
    include/dep0/source.hpp
    include/dep0/temp_file.hpp
    include/dep0/unique_ref.hpp
    include/dep0/vector_splice.hpp
    src/destructive_self_assign.cpp
    src/digit_separator.cpp
    src/error.cpp
    src/fmap.cpp
    src/match.cpp
    src/mmap.cpp
    src/scope_map.cpp
    src/source.cpp
    src/temp_file.cpp
    src/unique_ref.cpp
    src/vector_splice.cpp
    )
add_library(DepC::Dep0::Core ALIAS dep0_core_lib)
target_compile_features(dep0_core_lib PUBLIC cxx_std_20)
target_include_directories(dep0_core_lib PUBLIC include)
target_link_libraries(dep0_core_lib PRIVATE Boost::Boost)
