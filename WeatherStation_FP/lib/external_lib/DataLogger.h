#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include "DataLogger_interface.h"

class DataLogger: public DataLogger_interface {
    private:

    public:
        void logEspLog(const char* msg1, const char* msg2);
};

#endif // DATA_LOGGER_H