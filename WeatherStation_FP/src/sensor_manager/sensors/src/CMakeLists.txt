# Create the sensors library with the source files
add_library(SensorSrc soil_moisture.cpp)

# Link with the Sensors library
target_link_libraries(SensorSrc Sensors)
