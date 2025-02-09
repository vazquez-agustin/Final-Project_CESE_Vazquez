# Create the sensors library with the source files
add_library(SoilMoisture soil_moisture.cpp)

target_include_directories(SoilMoisture PUBLIC "./")

# Link with the Sensors library
target_link_libraries(SoilMoisture Sensors)

target_include_directories(${COMPONENT_TARGET} PUBLIC .)