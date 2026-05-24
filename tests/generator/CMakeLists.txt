add_library(generator
    src/TestGenerator.cpp
)

target_include_directories(generator
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(generator
    PUBLIC common
)
