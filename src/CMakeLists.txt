add_subdirectory(common)

add_executable(apsp_main main.cpp)

target_link_libraries(apsp_main
    PRIVATE common
)
