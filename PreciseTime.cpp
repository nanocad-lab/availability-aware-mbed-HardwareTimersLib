/* PreciseTime.cpp
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */
 
#include "mbed.h"
#include "PreciseTime.h"

PreciseTime::PreciseTime() :
                h(0),
                m(0),
                s(0),
                ms(0),
                us(0),
                ns(0)
                {}
                
                
void PreciseTime::print() {
   // printf("HH:MM:SS:ms:us:ns\r\n");
   printf("%02u:%02u:%02u:%03u:%03u:%03u", h, m, s, ms, us, ns); 
}

uint32_t PreciseTime::to_h(PreciseTime obj) {
    return obj.h;
}

uint32_t PreciseTime::to_m(PreciseTime obj) {
    return to_h(obj)*MIN_PER_HOUR + obj.m;
}

uint32_t PreciseTime::to_s(PreciseTime obj) {
    return to_m(obj)*SEC_PER_MIN + obj.s;
}

uint32_t PreciseTime::to_ms(PreciseTime obj) {
    return to_s(obj)*MS_PER_SEC + obj.ms;
}

uint32_t PreciseTime::to_us(PreciseTime obj) {
    return to_ms(obj)*US_PER_MS + obj.us;
}

uint32_t PreciseTime::to_ns(PreciseTime obj) {
    return to_us(obj)*NS_PER_US + obj.ns;
}

PreciseTime PreciseTime::from_h(uint32_t h) {
    PreciseTime obj;
    obj.h = h;
    return obj;
}

PreciseTime PreciseTime::from_m(uint32_t m) {
    PreciseTime obj;
    obj.h = m * HOUR_PER_MIN;
    obj.m = m % MIN_PER_HOUR;
    return obj;    
}

PreciseTime PreciseTime::from_s(uint32_t s) {
    PreciseTime obj;
    obj.m = s * MIN_PER_SEC;
    obj.s = s % SEC_PER_MIN;
    
    obj.h = obj.m * HOUR_PER_MIN;
    obj.m = obj.m % MIN_PER_HOUR;
    return obj;     
}

PreciseTime PreciseTime::from_ms(uint32_t ms) {
    PreciseTime obj;
    obj.s = ms * SEC_PER_MS;
    obj.ms = ms % MS_PER_SEC;
    
    obj.m = obj.s * MIN_PER_SEC;
    obj.s = obj.s % SEC_PER_MIN;
    
    obj.h = obj.m * HOUR_PER_MIN;
    obj.m = obj.m % MIN_PER_HOUR;
    return obj; 
}

PreciseTime PreciseTime::from_us(uint32_t us) {
    PreciseTime obj;
    obj.ms = us * MS_PER_US;
    obj.us = us % US_PER_MS;
    
    obj.s = obj.ms * SEC_PER_MS;
    obj.ms = obj.ms % MS_PER_SEC;
    
    obj.m = obj.s * MIN_PER_SEC;
    obj.s = obj.s % SEC_PER_MIN;
    
    obj.h = obj.m * HOUR_PER_MIN;
    obj.m = obj.m % MIN_PER_HOUR;
    return obj;    
}

PreciseTime PreciseTime::from_ns(uint32_t ns) {
    PreciseTime obj;
    obj.us = ns * US_PER_NS;
    obj.ns = ns % NS_PER_US;
    
    obj.ms = obj.us * MS_PER_US;
    obj.us = obj.us % US_PER_MS;
    
    obj.s = obj.ms * SEC_PER_MS;
    obj.ms = obj.ms % MS_PER_SEC;
    
    obj.m = obj.s * MIN_PER_SEC;
    obj.s = obj.s % SEC_PER_MIN;
    
    obj.h = obj.m * HOUR_PER_MIN;
    obj.m = obj.m % MIN_PER_HOUR;
    return obj;     
}