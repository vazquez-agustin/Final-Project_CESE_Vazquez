#ifndef DELAY_MANAGER_H
#define DELAY_MANAGER_H

#include "DelayManager_interface.h"

class DelayManager: public DelayManager_interface {
    private:

    public:
        void delay(uint32_t ms);
};

#endif // DELAY_MANAGER_H