/* Timer_LPTMR.cpp
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#include "mbed.h"
#include "Timer_LPTMR.h"
#include "PreciseTime.h"


//Init Timer_LPTMR class variables
bool Timer_LPTMR::__lptmr_used = false;
Timer_LPTMR *Timer_LPTMR::__obj = NULL;

Timer_LPTMR::Timer_LPTMR() :
        HardwareTimer(0xFFFF, 1, HardwareTimer::ms) //LPTMR has 16-bit counter. And at 1 KHz, each clock cycle is 1 ms
        {   
    if (__lptmr_used)
        __valid = false;
    else {
        __valid = true;
        __lptmr_used = true;
        __obj = this;
    }
}

Timer_LPTMR::~Timer_LPTMR() {
    if (__valid) {
        __lptmr_used = false; //free the hardware LPTMR resource
        __obj = NULL;
    }
}

uint32_t Timer_LPTMR::getTick() {
    if (!__valid)
        return 0;
    
    uint16_t ticks;
    uint32_t count;
    
    //__disable_irq(); //CRITICAL SECTION -- ALL INTERRUPTS MUST BE STOPPED!
    LPTMR0->CNR = 0; //need to write to the register in order to read it due to buffering
    ticks = (uint16_t) LPTMR0->CNR;
    
    //It is theoretically possible that the timer interrupt is invoked here, between these two lines.
    //However, the timer doesn't appear to work properly if I put these two in a critical section.
    
    count = __count;
    //__enable_irq(); //END CRITICAL SECTION
    
    //Convert to ticks
    return (uint32_t) count * __rolloverValue + ticks;
}

void Timer_LPTMR::__init_timer() {    
    //MCG clocks  
    MCG->C2 &= ~MCG_C2_IRCS_MASK; //Set slow internal reference clk (32 KHz)
    MCG->C1 |= MCG_C1_IRCLKEN_MASK; //Enable internal reference clk (MCGIRCLK)
   // MCG->C1 |= MCG_C1_IREFSTEN_MASK; //Enable internal reference clk (MCGIRCLK) to work in stop mode
    
    //Timer clock gating
    SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; //Disable clock gating the timer
    
    //Timer prescaling and clock selection
    LPTMR0->PSR = LPTMR_PSR_PCS(0); //Set LPTMR0 to use MCGIRCLK --> 32 KHz
    LPTMR0->PSR |= LPTMR_PSR_PRESCALE(4); // divide by 32 to get 1 KHz
    
    //Status reset
    LPTMR0->CSR = 0; //Reset the timer control/status register
    
    //Set interrupt handler
    NVIC_SetVector(LPTimer_IRQn, (uint32_t) __lptmr_isr_wrapper);
    NVIC_EnableIRQ(LPTimer_IRQn);
    
    //Good to go!
}

void Timer_LPTMR::__start_timer() {
    LPTMR0->CMR = __rolloverValue; //Set the compare register
    LPTMR0->CSR |= LPTMR_CSR_TIE_MASK; //Enable interrupt
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK; //Start the timer
}

void Timer_LPTMR::__stop_timer() {
    LPTMR0->CSR = 0; //Reset the LPTMR timer control/status register
}

void Timer_LPTMR::__timer_isr() {
    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;  //Write 1 to TCF to clear the LPT timer compare flag
    if ((__periodic || __num_callbacks > 0) && __user_fptr != NULL) { //user callback
        __user_fptr->call();
        if (!__periodic)
            __num_callbacks--;
    }   
    __count++;
}

void Timer_LPTMR::__lptmr_isr_wrapper() {
    __obj->__timer_isr();   
}