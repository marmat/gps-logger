/**
 * \file global.h
 * \brief contains some useful constant definitions and macros that may be used 
 * throughout the project code.
 * \author Martin Matysiak
 */

#ifndef GLOBAL_H
    #define GLOBAL_H

    #ifndef F_CPU
        /// CPU frequency = 7,3728MHz (baud-rate crystal)
        #define F_CPU 7372800UL
    #endif

    /// Port to which input/output devices (LEDs etc.) are connected
    #define IO_PORT PORTC
    /// Direction register for the input/output port
    #define IO_CONF DDRC
    /// Pin of the status LED
    #define LED_STAT PC0

    /// Macro for turning the LED off
    #define LEDCODE_OFF() IO_PORT &= ~(1 << LED_STAT)
    /// Macro for turning the LED on
    #define LEDCODE_ON() IO_PORT |= (1 << LED_STAT)
    /// Macro for toggling the current LED state
    #define LEDCODE_BLINK() IO_PORT ^= (1 << LED_STAT)

    /// definition of 'true' when using 8-bit integer as boolean replacement
    #define TRUE 1
    /// definition of 'false'
    #define FALSE 0
    ///ASCII character no. 03 - End of Text
    #define ETX 0x03
    ///ASCII character no. 10 - Linefeed
    #define LF 0x0A    
    ///ASCII character no. 13 - Carriage Return
    #define CR 0x0D

    #include <stdint.h>
    #include <stdlib.h>
    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <util/delay.h>

    /** 
     * \brief Perform a break for a specified time of seconds
     *
     * Although the integrated _delay_ms would allow values large enough to 
     * establish seconds of delaying, the compiled code would be highly 
     * inefficient (i.e. it would take a lot of space). Therefore, several 
     * _delay_ms(250) calls in a row is a better approach for that
     *
     * \param pSeconds An integer containing the seconds that should be spend with
     * doing nothing
     */
    void _delay_s(uint8_t pSeconds);
#endif
