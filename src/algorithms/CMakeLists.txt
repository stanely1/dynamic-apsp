add_library(algorithms
    FloydWarshallAlgorithm.cpp
)

target_include_directories(algorithms
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
