add_library(SoilMoisture soil_moisture.cpp)

# Include the current directory and external_interface for ADC
target_include_directories(SoilMoisture PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../external_interface)

# Link SoilMoisture with ExternalInterface (for ADC functionality)
target_link_libraries(SoilMoisture ExternalInterface)


#add_library(SoilMoisture soil_moisture.cpp)

#target_include_directories(SoilMoisture PUBLIC "./")

#target_link_libraries(SoilMoisture Sensors)

#target_include_directories(${COMPONENT_TARGET} PUBLIC .)