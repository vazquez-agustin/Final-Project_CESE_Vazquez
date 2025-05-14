#ifndef DATA_HELPER_H
#define DATA_HELPER_H

#include <string>

std::string prepare_data(uint32_t moisture_value, uint32_t speed_value, float temperature_value, float pressure_value, float humidity_value) {
    std::string data = "humedad_de_suelo,location=outside value=" + std::to_string(moisture_value) + "\n" +
                            "velocidad_de_viento,location=outside value=" + std::to_string(speed_value) + "\n" +
                            "temperatura,location=outside value=" + std::to_string(temperature_value) + "\n" +
                            "humedad,location=outside value=" + std::to_string(humidity_value) + "\n" +
                            "presion,location=outside value=" + std::to_string(pressure_value);
    return data;
}

#endif // DATA_HELPER_H