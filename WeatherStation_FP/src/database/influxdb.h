#ifndef INFLUXDB_H
#define INFLUXDB_H

#include "parameters.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <cstring>
#include <string>

class InfluxDBClient {
public:
    void send_data(uint32_t moisture_value);

private:
    static esp_err_t http_event_handler(esp_http_client_event_t *evt);

    const char* INFLUXDB_BUCKET = "meditions";
    const char* INFLUXDB_ORG = "stationtest";
};

#endif // INFLUXDB_H
