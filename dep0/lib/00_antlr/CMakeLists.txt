#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
find_package(antlr4-runtime 4.13.0 REQUIRED)
find_package(Java 1.11 COMPONENTS Runtime REQUIRED)

set(cpp_files
    "dep0/antlr4/DepCLexer.cpp"
    "dep0/antlr4/DepCLexer.h"
    "dep0/antlr4/DepCParserBaseListener.cpp"
    "dep0/antlr4/DepCParserBaseListener.h"
    "dep0/antlr4/DepCParserBaseVisitor.cpp"
    "dep0/antlr4/DepCParserBaseVisitor.h"
    "dep0/antlr4/DepCParser.cpp"
    "dep0/antlr4/DepCParser.h"
    "dep0/antlr4/DepCParserListener.cpp"
    "dep0/antlr4/DepCParserListener.h"
    "dep0/antlr4/DepCParserVisitor.cpp"
    "dep0/antlr4/DepCParserVisitor.h"
    )
set(antlr4_dir ${CMAKE_SOURCE_DIR}/antlr4)
set(antlr4_jar_file ${antlr4_dir}/download/antlr-4.13.0-complete.jar)
add_custom_command(
    OUTPUT ${cpp_files}
    DEPENDS ${antlr4_dir}/DepCParser.g4 ${antlr4_dir}/DepCLexer.g4
    COMMAND ${Java_JAVA_EXECUTABLE}
    ARGS
        -jar ${antlr4_jar_file}
        -listener -visitor
        -Dlanguage=Cpp -package dep0 -o ${CMAKE_CURRENT_BINARY_DIR}/dep0/antlr4
        ${antlr4_dir}/DepCParser.g4 ${antlr4_dir}/DepCLexer.g4 
    )
add_library(dep0_antlr4_lib ${cpp_files})
add_library(DepC::Dep0::Antlr4 ALIAS dep0_antlr4_lib)
target_include_directories(dep0_antlr4_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(dep0_antlr4_lib PUBLIC antlr4_static)
