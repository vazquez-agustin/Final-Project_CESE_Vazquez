
add_library(API API.cpp)

target_include_directories(API PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${COMPONENT_TARGET} PUBLIC .)