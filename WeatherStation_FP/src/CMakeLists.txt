FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(
    SRCS 
        "main.cpp"
        "API/API.cpp"
        "database/influxdb.cpp"
        "sensor_manager/sensor_manager.cpp"
        "sensor_manager/sensors/soil_moisture/soil_moisture.cpp"
        "sensor_manager/sensors/wind_speed/wind_speed.cpp"
        "sensor_manager/sensors/bme_680/BME680.cpp"
        "external_interface/I2C.cpp"
        "sensor_manager/sensors/wind_direction/wind_direction.cpp"
        "external_interface/ADC.cpp"
        "external_interface/WiFi.cpp"

    INCLUDE_DIRS 
        "."
        "API"
        "sensor_manager"
        "sensor_manager/sensors"
        "sensor_manager/sensors/soil_moisture"
        "sensor_manager/sensors/wind_speed"
        "sensor_manager/sensors/bme_680"
        "sensor_manager/sensors/wind_direction"
        "external_interface"
        "database"
)