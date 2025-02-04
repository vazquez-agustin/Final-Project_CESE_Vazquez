# Create the sensors library with the source files
add_library(SoilMosture soil_moisture.cpp)

target_include_directories(SoilMosture PUBLIC "./")

# Link with the Sensors library
target_link_libraries(SoilMosture Sensors)

target_include_directories(${COMPONENT_TARGET} PUBLIC .)