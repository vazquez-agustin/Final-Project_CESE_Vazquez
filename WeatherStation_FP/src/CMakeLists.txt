# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(
    SRCS 
        "main.cpp"
        "API/API.cpp"
        "database/influxdb.cpp"
        "sensor_manager/manager.cpp"
        "sensor_manager/sensors/soil_mosture/soil_moisture.cpp"
        "external_interface/ADC.cpp"
        "external_interface/WiFi.cpp"
    INCLUDE_DIRS 
        "."
        "API"
        "sensor_manager"
        "sensor_manager/sensors/soil_mosture"
        "external_interface"
)