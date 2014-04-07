/* Timer_TPM.cpp
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#include "mbed.h"
#include "HardwareTimer.h"
#include "Timer_TPM.h"

//Init Timer_TPM class variables
bool Timer_TPM::__tpm_used = false;
Timer_TPM *Timer_TPM::__obj = NULL;

Timer_TPM::Timer_TPM() :
        HardwareTimer(0xFFFF, 20.833333333, HardwareTimer::ns) //TPM has 16-bit counter. And at 48MHz, each clock cycle is 20.8333333 ns
        {   
    if (__tpm_used)
        __valid = false;
    else {
        __valid = true;
        __tpm_used = true;
        __obj = this;
    }
}

Timer_TPM::~Timer_TPM() {
    if (__valid) {
        __tpm_used = false; //free the hardware TPM resource
        __obj = NULL;
    }
}

uint32_t Timer_TPM::getTick() {
    if (!__valid)
        return 0;
        
    uint16_t tick;
    uint32_t count;
    
    //__disable_irq(); //CRITICAL SECTION -- ALL INTERRUPTS MUST BE STOPPED!
    TPM0->CNT = 0; //Need to write to the TPM CNT register in order to latch current value.
    tick = (uint16_t) TPM0->CNT; //Value was previously latched, just read it

    //It is theoretically possible that the timer interrupt is invoked here, between these two lines.
    //However, the timer doesn't appear to work properly if I put these two in a critical section.
    
    count = __count; //Latch current count value.
    //__enable_irq(); //END CRITICAL SECTION
    
    //Convert to ticks
    return count * __rolloverValue + (uint32_t) tick;
}

void Timer_TPM::__init_timer() {    
    //Set TPM clocks
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //Set TPM global clock source: MCGFLLCLK
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; //Enable TPM block (clock gating)

    TPM0->SC = 0; //Reset TPM
    
    //Configure TPM prescaler
    TPM0->SC = TPM_SC_PS(0); //Prescaler set to 1
    
    TPM0->CNT = 0; //Set the count register
    
    //Set interrupt handler
    NVIC_SetVector(TPM0_IRQn, (uint32_t) __tpm_isr_wrapper);
    NVIC_EnableIRQ(TPM0_IRQn);
}

void Timer_TPM::__start_timer() {
    TPM0->MOD = (uint16_t) __rolloverValue; //Set the modulo register
    TPM0->SC |= TPM_SC_TOIE_MASK; //Enable interrupt
    TPM0->SC |= TPM_SC_CMOD(1); //Start the timer. Timer will increment on the TPM clock edges, not an external clock
}

void Timer_TPM::__stop_timer() {
    TPM0->SC = 0; //Reset TPM
}

void Timer_TPM::__timer_isr() {
    TPM0->SC |= TPM_SC_TOF_MASK;
    TPM0->MOD = (uint16_t) __rolloverValue; //Set the modulo register
    if ((__periodic || __num_callbacks > 0) && __user_fptr != NULL) { //user callback
        __user_fptr->call();
        if (!__periodic)
            __num_callbacks--;
    }   
    __count++;
}

void Timer_TPM::__tpm_isr_wrapper() {
    __obj->__timer_isr();   
}