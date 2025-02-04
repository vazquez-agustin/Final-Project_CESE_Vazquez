# Create the SensorManager library
add_library(SensorManager manager.cpp)

# Add sensors subdirectory
add_subdirectory(sensors)
add_subdirectory(${CMAKE_SOURCE_DIR}/external_interface) # Add external_interface subdirectory

# Ensure SensorManager can find external_interface headers
target_include_directories(SensorManager PUBLIC ${CMAKE_SOURCE_DIR}/external_interface)

# Specify dependencies
target_link_libraries(SensorManager PRIVATE Sensors ExternalInterface)
target_include_directories(${COMPONENT_TARGET} PUBLIC .)