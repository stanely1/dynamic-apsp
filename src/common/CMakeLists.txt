add_library(common
    Graph.cpp
)

target_include_directories(common
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
