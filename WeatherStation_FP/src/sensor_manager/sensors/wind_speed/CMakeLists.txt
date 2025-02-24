add_library(WindSpeed wind_speed.cpp)

# Include the current directory and external_interface for ADC
target_include_directories(WindSpeed PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../external_interface)

# Link SoilMoisture with ExternalInterface (for ADC functionality)
target_link_libraries(WindSpeed ExternalInterface)
