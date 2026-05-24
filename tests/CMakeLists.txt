add_subdirectory(generator)

add_executable(apsp_tests runner.cpp)

target_link_libraries(apsp_tests
    PRIVATE algorithms
    PRIVATE common
    PRIVATE generator
)

add_test(
    NAME apsp_tests
    COMMAND apsp_tests
)
