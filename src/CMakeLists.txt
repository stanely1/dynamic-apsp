add_subdirectory(algorithms)
add_subdirectory(common)
add_subdirectory(generator)

add_executable(apsp_main main.cpp)

target_link_libraries(apsp_main
    PRIVATE common
    PRIVATE algorithms
    PRIVATE generator
)
