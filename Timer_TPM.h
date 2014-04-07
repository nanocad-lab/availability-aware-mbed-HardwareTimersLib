/* Timer_TPM.h
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#ifndef TIMER_TPM_H
#define TIMER_TPM_H

#include "mbed.h"
#include "HardwareTimer.h"
#include "PreciseTime.h"

/**
 * Base class for TPM timing on the FRDM-KL46Z.
 */
class Timer_TPM : public HardwareTimer {
    public:
        /**
         * Construct a new TPM timer. The timer operates at 48MHz. Only one Timer_TPM
         * object may be valid at a time (can control hardware).
         */
        Timer_TPM();
        
        /**
         * Destroy the TPM object. If the object was valid (was allowed to access the timer
         * hardware), it will free the resource so that another Timer_TPM object can access
         * hardware.
         */
        virtual ~Timer_TPM();

        virtual uint32_t getTick();
    
    private:        
        virtual void __init_timer();
        virtual void __start_timer();
        virtual void __stop_timer();
        virtual void __timer_isr();
        
        /** 
         * We need a static function to use as interrupt service routine.
         * Although we would ideally like to use a member function of Timer_TPM,
         * we need to wrap it instead.
         */
        static void __tpm_isr_wrapper();
                
        static bool __tpm_used; //This flag ensures that no two Timer_TPM objects attempt to manipulate the hardware at once
        static Timer_TPM *__obj; //if __tpm_used is true, this should point to the valid Timer_TPM object. This helps with the ISR wrapper.
};

#endif