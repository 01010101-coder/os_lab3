add_library(sync_utils
    SharedResources.cpp
    MarkerThread.cpp
)

target_include_directories(sync_utils
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(Lab3Sync Main.cpp)
target_link_libraries(Lab3Sync sync_utils)