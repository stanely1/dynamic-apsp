cmake_minimum_required(VERSION 3.20)

project(DynamicAPSP
    VERSION 1.0
    LANGUAGES CXX
)

enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# TODO: compiler flags (for optimization)
add_compile_options(-Wall -Wextra)# -Wshadow)

add_subdirectory(src)
add_subdirectory(tests)
