add_library(generator
    TestGenerator.cpp
)

target_include_directories(generator
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
