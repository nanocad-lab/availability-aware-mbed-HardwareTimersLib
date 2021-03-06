/* Timer_LPTMR.h
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#ifndef TIMER_LPTMR_H
#define TIMER_LPTMR_H

#include "mbed.h"
#include "HardwareTimer.h"
#include "PreciseTime.h"

/**
 * Base class for LPTMR timing on the FRDM-KL46Z.
 */
class Timer_LPTMR : public HardwareTimer {
    public:
        /**
         * Construct a new LPTMR timer. The timer operates at 1 KHz. Only one Timer_LPTMR
         * object may be valid at a time (can control hardware).
         */
        Timer_LPTMR();
        
        /**
         * Destroy the LPTMR object. If the object was valid (was allowed to access the timer
         * hardware), it will free the resource so that another Timer_LPTMR object can access
         * hardware.
         */
        virtual ~Timer_LPTMR();

        virtual uint32_t getTick();
    
    private:        
        virtual void __init_timer();
        virtual void __start_timer();
        virtual void __stop_timer();
        virtual void __timer_isr();
        
        /** 
         * We need a static function to use as interrupt service routine.
         * Although we would ideally like to use a member function of Timer_LPTMR,
         * we need to wrap it instead.
         */
        static void __lptmr_isr_wrapper();
                
        static bool __lptmr_used; //This flag ensures that no two Timer_LPTMR objects attempt to manipulate the hardware at once
        static Timer_LPTMR *__obj; //if __tpm_used is true, this should point to the valid Timer_LPTMR object. This helps with the ISR wrapper.
};

#endif