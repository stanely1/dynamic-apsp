add_library(structures
    PathPriorityQueue.cpp
)

target_include_directories(structures
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
