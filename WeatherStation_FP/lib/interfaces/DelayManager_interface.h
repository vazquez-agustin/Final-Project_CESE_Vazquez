#ifndef DELAY_MANAGER_INTERFACE_H
#define DELAY_MANAGER_INTERFACE_H

#include <cstdint>

class DelayManager_interface {       

    public:

        virtual void delay(uint32_t ms) = 0;
        
    };

#endif // DELAY_MANAGER_INTERFACE_H