# Create the sensors library with the source files
add_library(WindSpeed wind_speed.cpp)

target_include_directories(WindSpeed PUBLIC "./")

# Link with the Sensors library
target_link_libraries(WindSpeed Sensors)

target_include_directories(${COMPONENT_TARGET} PUBLIC .)