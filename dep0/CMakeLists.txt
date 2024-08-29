find_package(Boost 1.76.0 REQUIRED)

add_subdirectory(app/dep0)

add_subdirectory(lib/00_antlr)
add_subdirectory(lib/00_core)
add_subdirectory(lib/01_ast)
add_subdirectory(lib/02_parser)
add_subdirectory(lib/03_typecheck)
add_subdirectory(lib/04_transform)
add_subdirectory(lib/05_llvmgen)
add_subdirectory(lib/06_linker)
add_subdirectory(lib/99_testing)
