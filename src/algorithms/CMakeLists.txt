add_subdirectory(structures)

add_library(algorithms
    DijkstraAlgorithm.cpp
    FloydWarshallAlgorithm.cpp
)

target_include_directories(algorithms
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(algorithms
    PRIVATE structures
)
