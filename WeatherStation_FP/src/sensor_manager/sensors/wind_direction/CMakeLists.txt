add_library(WindDirection wind_direction.cpp)

# Include the current directory and external_interface for ADC
target_include_directories(WindDirection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../external_interface)

# Link SoilMoisture with ExternalInterface (for ADC functionality)
target_link_libraries(WindDirection ExternalInterface)
