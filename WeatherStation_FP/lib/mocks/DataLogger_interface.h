#ifndef DATA_LOGGER_INTERFACE_H
#define DATA_LOGGER_INTERFACE_H

class DataLogger_interface {       

    public:

        virtual void logEspLog(const char* msg1, const char* msg2) = 0;
        
    };

#endif // DATA_LOGGER_INTERFACE_H