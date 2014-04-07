/* Timer_PIT.cpp
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#include "mbed.h"
#include "HardwareTimer.h"
#include "Timer_PIT.h"


//Init Timer_PIT class variables
bool Timer_PIT::__pit_used = false;
Timer_PIT *Timer_PIT::__obj = NULL;

Timer_PIT::Timer_PIT() :
        HardwareTimer(0xFFFFFFFF, 41.666666666, HardwareTimer::ns) //PIT has 32-bit counter. And at 24 MHz, each clock cycle is 41.666666 ns
        {   
    if (__pit_used)
        __valid = false;
    else {
        __valid = true;
        __pit_used = true;
        __obj = this;
    }
}

Timer_PIT::~Timer_PIT() {
    if (__valid) {
        __pit_used = false; //free the hardware PIT resource
        __obj = NULL;
    }
}

uint32_t Timer_PIT::getTick() {
    if (!__valid)
        return 0;
        
    uint32_t tick;
    uint32_t count;

    //__disable_irq(); //CRITICAL SECTION -- ALL INTERRUPTS MUST BE STOPPED!
    tick = PIT->CHANNEL[0].CVAL; //counts down
    
    //It is theoretically possible that the timer interrupt is invoked here, between these two lines.
    //However, the timer doesn't appear to work properly if I put these two in a critical section.
    
    count = __count;
    //__enable_irq(); //END CRITICAL SECTION
        
    //Convert to ticks
    return (__rolloverValue - tick) + count * __rolloverValue;
}

void Timer_PIT::__init_timer() {        
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;   //Enable clocking of PIT
    
    PIT->MCR |= PIT_MCR_MDIS_MASK; //Setting MDIS bit disables the timer module.
    
    //Set interrupt handler
    NVIC_SetVector(PIT_IRQn, (uint32_t) __pit_isr_wrapper);
    NVIC_EnableIRQ(PIT_IRQn);
    
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; //Enable interrupts
    
    PIT->MCR &= ~PIT_MCR_MDIS_MASK; //Clearing MDIS bit enables the timer module.

    //Good to go!
}

void Timer_PIT::__start_timer() {
    PIT->CHANNEL[0].LDVAL = __rolloverValue; //Load the countdown value. PIT counts downwards.
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; //Enable the timer.
}

void Timer_PIT::__stop_timer() {
    PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK; //Disable the timer.
}

void Timer_PIT::__timer_isr() {
    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK; //Clear the timer interrupt flag bit
    if ((__periodic || __num_callbacks > 0) && __user_fptr != NULL) { //user callback
        __user_fptr->call();
        if (!__periodic)
            __num_callbacks--;
    }   
    __count++;
}

void Timer_PIT::__pit_isr_wrapper() {
    __obj->__timer_isr();   
}