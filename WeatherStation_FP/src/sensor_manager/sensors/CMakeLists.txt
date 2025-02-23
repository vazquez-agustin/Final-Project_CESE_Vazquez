add_library(Sensors STATIC soil_moisture.cpp wind_speed.cpp)

target_include_directories(Sensors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
