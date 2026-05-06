add_library(common
    Graph.cpp
    Path.cpp
)

target_include_directories(common
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
