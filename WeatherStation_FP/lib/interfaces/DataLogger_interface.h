#ifndef ESP_LOG_MOCK_H
#define ESP_LOG_MOCK_H

class DataLogger_interface {       

    public:

        virtual void logEspLog(char* msg1, char* msg2) = 0;
        
    };

#endif // ESP_LOG_MOCK_H