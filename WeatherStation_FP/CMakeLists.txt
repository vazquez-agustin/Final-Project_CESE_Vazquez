cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(WeatherStation_FP)

# Project subdirectories 
add_subdirectory(sensor_manager)
add_subdirectory(API)
add_subdirectory(database)
add_subdirectory(external_interface)

# Link the libraries
target_link_libraries(MyProject PRIVATE SensorManager ExternalInterface)
