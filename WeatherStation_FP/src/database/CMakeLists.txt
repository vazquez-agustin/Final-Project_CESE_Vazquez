# Create the library
add_library(Database influxdb.cpp)

# Include header directories
target_include_directories(Database PUBLIC  
    ${CMAKE_CURRENT_SOURCE_DIR}  # Includes the database directory
    ${CMAKE_SOURCE_DIR}          # Includes the root directory (where parameters.h is)
)