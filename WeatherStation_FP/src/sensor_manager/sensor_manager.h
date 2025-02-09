#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "soil_moisture.h"
#include "wind_speed.h"
#include "WiFi.h"
#include "influxdb.h"

class SensorManager {
public:
    SensorManager();
    void sensorsRun();

private:
    SoilMoistureSensor soilSensor;
    WindSpeed windSensor;
    InfluxDBClient influxClient;
};

#endif // SENSOR_MANAGER_H
