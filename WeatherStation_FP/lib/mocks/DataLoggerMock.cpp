#include "DataLogger_interface.h"

class DataLoggerMock: public DataLogger_interface {
    private:

    public:
        void logEspLog(const char* msg1, const char* msg2) {
            return;
        } 
};