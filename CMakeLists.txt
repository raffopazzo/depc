#
# Copyright Raffaele Rossi 2023 - 2024.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.20)
project(
    DepC
    VERSION 0.0.1
    DESCRIPTION "Like C and C++ but with Dependent Types"
    LANGUAGES CXX
)

enable_testing()

add_subdirectory(dep0)
add_subdirectory(docs)
