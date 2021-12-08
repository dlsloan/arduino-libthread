#ifndef __critical_h
#define __critical_h

#include <Arduino.h>

class critical {
public:
    critical() {
        noInterrupts();
	    __sync_synchronize();
    }

    ~critical() {
        __sync_synchronize();
        interrupts();
    }
};

#endif