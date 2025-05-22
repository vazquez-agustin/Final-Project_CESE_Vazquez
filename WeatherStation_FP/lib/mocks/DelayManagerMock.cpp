#include "DelayManager_interface.h"

class DelayManagerMock: public DelayManager_interface {
    private:

    public:
        void delay(uint32_t ms) {
            return;
        }
};