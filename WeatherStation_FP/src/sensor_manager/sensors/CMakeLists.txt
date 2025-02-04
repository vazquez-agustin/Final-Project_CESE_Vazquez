# Create the sensors library with the source files
add_library(SensorSrc soil_moisture.cpp)

# Include the header directory
target_include_directories(SensorSrc PUBLIC ../include)

# Link with the Sensors library
target_link_libraries(SensorSrc Sensors)

