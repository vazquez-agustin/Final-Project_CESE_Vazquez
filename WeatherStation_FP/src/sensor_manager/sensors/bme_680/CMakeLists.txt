add_library(BME680 BME680.cpp)

# Include the current directory and external_interface for ADC
target_include_directories(BME680 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../external_interface)

# Link SoilMoisture with ExternalInterface (for ADC functionality)
target_link_libraries(BME680 ExternalInterface)
