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

    void sensorsRun();
    void initWiFi();
    void initInfluxDB();

private:
    InfluxDBClient influxClient;
    WiFiManager WiFi;
    SoilMoistureSensor soilMoistureSensor;
    WindSpeedSensor windSpeedSensor;
};

#endif // SENSOR_MANAGER_H
