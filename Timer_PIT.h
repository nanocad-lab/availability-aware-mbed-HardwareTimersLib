/* Timer_PIT.h
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#ifndef TIMER_PIT_H
#define TIMER_PIT_H

#include "mbed.h"
#include "HardwareTimer.h"
#include "PreciseTime.h"

/**
 * Base class for PIT timing on the FRDM-KL46Z.
 */
class Timer_PIT : public HardwareTimer {
    public:
        /**
         * Construct a new PIT timer. The timer operates at 1 KHz. Only one Timer_PIT
         * object may be valid at a time (can control hardware).
         */
        Timer_PIT();
        
        /**
         * Destroy the PIT object. If the object was valid (was allowed to access the timer
         * hardware), it will free the resource so that another Timer_PIT object can access
         * hardware.
         */
        virtual ~Timer_PIT();

        virtual uint32_t getTick();
    
    private:        
        virtual void __init_timer();
        virtual void __start_timer();
        virtual void __stop_timer();
        virtual void __timer_isr();
        
        /** 
         * We need a static function to use as interrupt service routine.
         * Although we would ideally like to use a member function of Timer_PIT,
         * we need to wrap it instead.
         */
        static void __pit_isr_wrapper();
                
        static bool __pit_used; //This flag ensures that no two Timer_PIT objects attempt to manipulate the hardware at once
        static Timer_PIT *__obj; //if __tpm_used is true, this should point to the valid Timer_PIT object. This helps with the ISR wrapper.
};

#endif