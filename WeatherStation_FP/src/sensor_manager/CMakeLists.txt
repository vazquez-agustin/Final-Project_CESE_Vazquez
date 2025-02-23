add_library(SensorManager STATIC sensor_manager.cpp)

target_include_directories(SensorManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} sensors)

target_link_libraries(SensorManager Sensors)
