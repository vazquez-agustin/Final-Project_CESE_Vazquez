#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "soil_moisture.h"
#include "wind_speed.h"
#include "WiFi.h"
#include "influxdb.h"
#include "parameters.h"

class SensorManager {
public:
    SensorManager();
    void dbConnection();
    void sensorsRun();

private:
    InfluxDBClient influxClient;
    WiFiManager wifi;
    SoilMoistureSensor soilSensor;
    WindSpeed windSensor;
};

#endif // SENSOR_MANAGER_H
