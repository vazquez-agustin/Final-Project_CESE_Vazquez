#ifndef INFLUXDB_H
#define INFLUXDB_H

#include "esp_http_client.h"
#include <cstring>

class InfluxDBClient {
public:
    void send_data();

private:
    const char INFLUXDB_BUCKET = "meditions";
    const char INFLUXDB_ORG = "stationtest";
};

#endif // INFLUXDB_H