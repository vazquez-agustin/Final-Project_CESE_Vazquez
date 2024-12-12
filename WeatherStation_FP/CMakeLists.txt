cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(WeatherStation_FP)
include_directories(${CMAKE_CURRENT_LIST_DIR}/drivers/i2c/inc)