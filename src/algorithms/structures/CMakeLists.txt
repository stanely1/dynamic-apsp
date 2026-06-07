add_library(structures
    Path.cpp
    PathPriorityQueue.cpp
)

target_include_directories(structures
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(structures
    PRIVATE common
)
