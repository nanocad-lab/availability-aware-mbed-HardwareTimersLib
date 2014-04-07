/* HardwareTimer.h
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#ifndef HARDWARETIMER_H
#define HARDWARETIMER_H

#include "mbed.h"
#include "PreciseTime.h"

/**
 * This provides a base class from which actual hardware timers should derive their implementations.
 * This allows for a nice software interface regardless of the particular timer used.
 */
class HardwareTimer {
    public:
        typedef enum {
            ns,
            us,
            ms,
            s,
            m,
            h    
        } tick_units_t;
        
        /**
         * Constructs a new HardwareTimer.
         * @param valid if false, none of the timer functions can be used. This is intended to enforce only one object
         * managing a unique hardware timer.
         * @param maxRolloverTick maximum number of ticks in the hardware unit before it rolls over.
         * @param tickValue the amount of time corresponding to each timer tick, in units given by tickUnits.
         * @param tickUnits units for tickValue
         */
        HardwareTimer(uint32_t maxRolloverTick, float tickValue, tick_units_t tickUnits);
        
        /**
         * Destructs the HardwareTimer.
         */
        virtual ~HardwareTimer();
        
        /**
         * @returns true if this timer is valid. If false, the timer cannot be used.
         */
        bool valid();
        
        /**
         * @returns true if the timer is ready to start.
         */
        bool enabled(); 
        
        /**
         * @returns true if the timer is running.
         */
        bool running();   
        
        /**
         * @returns the amount of time corresponding to each timer tick in units given by tickUnits().
         */
        float tickValue();
        
        /**
         * @returns time in seconds corresponding to each tick
         */
        float tickUnits();
        
        /**
         * Enables the timer with a user-specified callback function that is called each time the timer expires.
         * @param fptr the user callback function
         */
        void enable(void (*fptr)(void));
        
        /**
         * Enables the timer with a user-specified callback function that is called each time the timer expires.
         * @param tptr the object 
         * @param mptr method to call on the object
         */
        template<typename T> void enable(T *tptr, void (T::*mptr)(void));
        
        /**
         * Stops and disables the timer. No user function callbacks will be made, and the tick value stops increasing.
         */
        void disable();
        
        /**
         * Starts the timer. If valid() or enabled() are false, then this method does nothing. Otherwise, the timer
         * begins ticking. The user callback function specified in enableTimer() is called each time the timer rolls over.
         * @param callback_tick_count the modulo tick value for when the timer calls the user callback function.
         * Note that the timer counts up. Note that some timers may not support the full 32-bit range. Use getMaxCallbackTickCount()
         * To check the maximum allowed value. If callback_tick_count is greater than that value, this method will have no effect.
         * @param periodic if true, the timer will call the user function every time the internal tick modulo callback_tick_count is reached.
         * If false, the user callback function is only called the first num_callbacks times.
         * @param num_callbacks if periodic is set to false, this many callbacks will be made. Before the timer stops.
         */
        void start(uint32_t callback_tick_count, bool periodic, uint32_t num_callbacks);
        
        /**
         * @returns the maximum value of the user-settable callback tick count (via startTimer()).
         * Some timers may support full 32-bit tick counts, while others may be less.
         */
        uint32_t getMaxCallbackTickCount();
        
        /**
         * Gets the timer value in a nice form.
         * Note that in general, the timer may overflow, leading to saturated values obtained from getTime().
         * To maximize resolution, accuracy, performance, and range, it is recommended to use
         * getTick() for most purposes. getTime() is mostly for convenience.
         * @returns the current tick converted into a PreciseTime representation.
         */
        PreciseTime getTime();
        
        /**
         * @returns the current tick number. Convert to seconds by multiplying the return value with tickValue().
         * Note that getTick() * tickValue() can easily overflow on faster timers due to the 32-bit upper bound
         * on arithmetic.
         */
        virtual uint32_t getTick() = 0;
        
        /**
         * Interrupt service routine for the timer. This should do timer hardware-specific chores before calling the user
         * callback function.
         */
        virtual void __timer_isr() = 0;
        
    protected:        
        /**
         * Initializes the particular hardware timer.
         */
        virtual void __init_timer() = 0;
        
        /**
         * Starts the particular hardware timer.
         */
        virtual void __start_timer() = 0;
        
        /**
         * Stop and disable the particular hardware timer.
         */
        virtual void __stop_timer() = 0;
        
        bool __valid; //timer can be used
        volatile uint32_t __count; //number of rollovers
        uint32_t __rolloverValue; //ticks per rollover
        bool __periodic; //periodic callbacks
        volatile uint32_t __num_callbacks;
        
        FunctionPointer *__user_fptr; //User callback function

    private:   
        bool __enabled; //timer is configured
        bool __running; //timer is running     
        uint32_t __maxRolloverTick; //maximum number of ticks before timer hardware rolls over
        float __tickValue; //how many units per tick
        tick_units_t __tickUnits; //tick units
};

#endif