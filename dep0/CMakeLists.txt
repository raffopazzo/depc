find_package(Boost 1.76.0 REQUIRED)

add_subdirectory(00_antlr_lib)
add_subdirectory(00_core_lib)
add_subdirectory(01_ast_lib)
add_subdirectory(02_parser_lib)
add_subdirectory(03_typecheck_lib)
add_subdirectory(04_llvmgen_lib)
