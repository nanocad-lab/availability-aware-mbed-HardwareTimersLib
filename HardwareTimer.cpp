/* HardwareTimer.cpp
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#include "mbed.h"
#include "HardwareTimer.h"

HardwareTimer::HardwareTimer(uint32_t maxRolloverTick, float tickValue, tick_units_t tickUnits) :
                    __valid(false),
                    __count(0),
                    __rolloverValue(0),
                    __periodic(false),
                    __num_callbacks(0),
                    __user_fptr(NULL),
                    __enabled(false),
                    __running(false),
                    __maxRolloverTick(maxRolloverTick),
                    __tickValue(tickValue),
                    __tickUnits(tickUnits)
                    {
}

HardwareTimer::~HardwareTimer() {
    disable();
    if (__user_fptr != NULL) //double check
        delete __user_fptr;    
}

bool HardwareTimer::valid() {
    return __valid;
}

bool HardwareTimer::enabled() {
    return __enabled;
}

bool HardwareTimer::running() {
    return __running;
}

float HardwareTimer::tickValue() {
    return __tickValue;   
}

float HardwareTimer::tickUnits() {
    switch (__tickUnits) {
        default:
        case ns:
            return 1e-9;
        case us:
            return 1e-6;
        case ms:
            return 1e-3;
        case s:
            return 1;
        case m:
            return 60;
        case h:
            return 3600;
    }   
}

void HardwareTimer::enable(void (*fptr)(void)) {
    if (!__valid)
        return;
    
    //set user function pointer
    if (__user_fptr != NULL)
        delete __user_fptr;
    if (fptr != NULL)
        __user_fptr = new FunctionPointer(fptr);

    __init_timer(); //Do hardware-specific initialization
        
    __enabled = true;
}


template <typename T> void HardwareTimer::enable(T *tptr, void (T::*mptr)(void)) {
    if (!__valid)
        return;
        
    if (__enabled)
        disable();
        
    //set user function pointer
    if (__user_fptr != NULL)
        delete __user_fptr;
    if (tptr != NULL && mptr != NULL)
        __user_fptr = new FunctionPointer(tptr, mptr);

    __init_timer(); //Do hardware-specific initialization
    
    __enabled = true;
}

void HardwareTimer::disable() {
    if (!__valid)
        return;
        
    __stop_timer(); //Do hardware-specific stop
    __running = false;
    
    if (__user_fptr != NULL) //Detach user callback function
        delete __user_fptr;
    __user_fptr = NULL;
        
    __enabled = false;
}

void HardwareTimer::start(uint32_t callback_tick_count, bool periodic, uint32_t num_callbacks) {
    if (!__valid || !__enabled || callback_tick_count > __maxRolloverTick)
        return;
    
    __rolloverValue = callback_tick_count;
    __periodic = periodic;
    if (__periodic)
        __num_callbacks = 0;
    else
        __num_callbacks = num_callbacks;
    
    __start_timer(); //Do hardware-specific start
    __running = true;
}

uint32_t HardwareTimer::getMaxCallbackTickCount() {
    return __maxRolloverTick;
}

PreciseTime HardwareTimer::getTime() {
    if (!__valid)
        return PreciseTime();
    
    uint32_t tick = getTick();
    uint32_t converted_ticks = tick * __tickValue;
    
    switch (__tickUnits) {
        default:
        case ns:
            return PreciseTime::from_ns(converted_ticks);
        case us:
            return PreciseTime::from_us(converted_ticks);
        case ms:
            return PreciseTime::from_ms(converted_ticks);
        case s:
            return PreciseTime::from_s(converted_ticks);
        case m:
            return PreciseTime::from_m(converted_ticks);
        case h:
            return PreciseTime::from_h(converted_ticks);
    }
}