/* PreciseTime.h
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#ifndef PRECISETIME_H
#define PRECISETIME_H

#include "mbed.h"

/**
 * This class provides a simple abstraction for time-keeping in a wall-clock sense.
 * It can store time by hour:min:sec:ms:us:ns, and convert time counts into a wall-clock
 * time and vice versa.
 */
class PreciseTime {
    public:
        PreciseTime();
        
        /**
         * Prints an ASCII representation of this object.
         */
        void print();
        
        /**
         * Convert a PreciseTime object to hours.
         * @param obj the object to convert
         * @returns value of obj in hours
         */
        static uint32_t to_h(PreciseTime obj);
        
        /**
         * Convert a PreciseTime object to minutes.
         * @param obj the object to convert
         * @returns value of obj in minutes
         */
        static uint32_t to_m(PreciseTime obj);
        
        /**
         * Convert a PreciseTime object to seconds.
         * @param obj the object to convert
         * @returns value of obj in seconds
         */
        static uint32_t to_s(PreciseTime obj);
        
        /**
         * Convert a PreciseTime object to ms
         * @param obj the object to convert
         * @returns value of obj in ms
         */
        static uint32_t to_ms(PreciseTime obj);
        
        /**
         * Convert a PreciseTime object to us.
         * @param obj the object to convert
         * @returns value of obj in us
         */
        static uint32_t to_us(PreciseTime obj);
        
        /**
         * Convert a PreciseTime object to ns.
         * @param obj the object to convert
         * @returns value of obj in ns
         */
        static uint32_t to_ns(PreciseTime obj);
        
        /**
         * Convert an integer number of hours to a PreciseTime representation.
         * @param h number of hours
         * @returns the PreciseTime representation
         */
        static PreciseTime from_h(uint32_t h);
        
        /**
         * Convert an integer number of minutes to a PreciseTime representation.
         * @param m number of minutes
         * @returns the PreciseTime representation
         */
        static PreciseTime from_m(uint32_t m);
        
        /**
         * Convert an integer number of seconds to a PreciseTime representation.
         * @param s number of seconds
         * @returns the PreciseTime representation
         */
        static PreciseTime from_s(uint32_t s);
        
        /**
         * Convert an integer number of ms to a PreciseTime representation.
         * @param ms number of ms
         * @returns the PreciseTime representation
         */
        static PreciseTime from_ms(uint32_t ms);
        
        /**
         * Convert an integer number of us to a PreciseTime representation.
         * @param us number of us
         * @returns the PreciseTime representation
         */
        static PreciseTime from_us(uint32_t us);
        
        /**
         * Convert an integer number of ns to a PreciseTime representation.
         * @param ns number of ns
         * @returns the PreciseTime representation
         */
        static PreciseTime from_ns(uint32_t ns);
        
        uint32_t h;
        uint32_t m;
        uint32_t s;
        uint32_t ms;
        uint32_t us;
        uint32_t ns;
        
        //constants for time conversion since we do not have floating point division
        const static uint32_t NS_PER_US = 1000;
        const static uint32_t US_PER_MS = 1000;
        const static uint32_t MS_PER_SEC = 1000;
        const static uint32_t SEC_PER_MIN = 60;
        const static uint32_t MIN_PER_HOUR = 60;
        
        const static float US_PER_NS = 0.001;
        const static float MS_PER_US = 0.001;
        const static float SEC_PER_MS = 0.001;
        const static float MIN_PER_SEC = 0.016666666666;
        const static float HOUR_PER_MIN = 0.016666666666;
};

#endif