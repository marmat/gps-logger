/**
 * \file gLogger.c
 * \brief The source file containing the main method which is called when
 * turning on the device.
 * \author Martin Matysiak
 */

#include <avr/sleep.h>
#include "global.h"
#include "modules/nofs.h"
#include "modules/gps.h"

/**
 * \brief Main method of the project
 *
 * This method contains the general workflow of the device. This is the place
 * where the different modules are combined in order to acheive the actual
 * logging of GPS-coordinates.
 *
 * \return Despite of being defined as a function returning an integer, the
 * method will return nothing, as the code will stay in a while(TRUE)-loop 
 * prior to the return statement.
 */
int main (void) {
    // Activate interrupts
    sei();  

    // Configure ports
    IO_CONF |= (1 << LED_STAT);
    LEDCODE_ON();

    // Disable unnecessary functions (Power-reduction)
    PRR |= (1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRADC);

    // Wait a second...
    _delay_s(1);

    // Initialize modules (if either one returns FALSE, the code will terminate
    // here in the while(TRUE)-loop, signalizing an error to the user)
    if (!(nofs_init() && gps_init())) {
        while(TRUE) {
            _delay_ms(100);
            LEDCODE_BLINK();    
        }
    }

    LEDCODE_OFF();

    char nmeaBuf[128];

    while(TRUE) {
        // We'll write the data only if it contains a valid position
        if (gps_getNmeaSentence(nmeaBuf, 128)) {
            nofs_writeString(nmeaBuf);
        }

        // Flash !!! (the most important part of the code)
        LEDCODE_BLINK();
        _delay_ms(30);
        LEDCODE_BLINK(); 
        sleep_mode();
    }

    // Will never be reached
    return 0;
}
