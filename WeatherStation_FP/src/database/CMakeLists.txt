# Create the library
add_library(Database influxdb.cpp)

# Include header directories
target_include_directories(Database PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})