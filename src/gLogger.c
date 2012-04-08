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

////////////////////////////////////////////////////////////////////////////////
// Change these constants in order to alter the logging behaviour

/**
 * Indicates how many message packets per second shall be recorded 
 * Valid values for the ST22 are: [1, 2, 4, 8, 10]
 */
#define FREQUENCY 1

/**
 * Indicates which messages to record per message packet
 * Valid values for the ST22 are:
 * [GPS_NMEA_GGA, GPS_NMEA_GSA, GPS_NMEA_GSV, GPS_NMEA_GLL, GPS_NMEA_RMC,
 *  GPS_NMEA_VTG, GPS_NMEA_ZDA]
 * Several message types can be combined using ORs.
 */
#define MESSAGES GPS_NMEA_GGA | GPS_NMEA_RMC | GPS_NMEA_VTG

// No changes needed after this point
////////////////////////////////////////////////////////////////////////////////

/// Number of set bits in the MESSAGES field (ugly code, but it serves its purpose)
#define NUM_MESSAGES (((MESSAGES) & 1) + (((MESSAGES) >> 1) & 1) + (((MESSAGES) >> 2) & 1) + (((MESSAGES) >> 3) & 1) + (((MESSAGES) >> 4) & 1) + (((MESSAGES) >> 5) & 1) + (((MESSAGES) >> 6) & 1) + (((MESSAGES) >> 7) & 1))

/// The LED will blink every LED_THRESHOLD messages
#define LED_THRESHOLD NUM_MESSAGES * FREQUENCY

char nmeaBuf[128];

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

    // Disable unneccesary modules
    PRR |= (1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRADC);

    _delay_ms(100);

    // Initialize the necessary modules (these methods may lock the processor
    // in an endless loop if an error occurs!)
    nofs_init();
    gps_init(FREQUENCY, MESSAGES);

    // Write a short information string containing the firmware version (NMEA compliant)
    nofs_writeString("\r\n$PGLGVER,1.6\r\n");

    // Keep track of received messages
    uint8_t messageCount = 0;
    LEDCODE_OFF();

    while(1) {
        // We'll write the data only if it contains a valid position
        if (gps_getNMEA(nmeaBuf, 128) & GPS_NMEA_VALID) {
            nofs_writeString(nmeaBuf);
        }

        // Makes sure that the LED is blinking only roughly once a second
        if (++messageCount == LED_THRESHOLD) {
            // Flash !!! (the most important part of the code)
            LEDCODE_BLINK();
            _delay_ms(30);
            LEDCODE_BLINK();
            messageCount = 0;
        }        
        
        sleep_mode();
    }

    // Will never be reached
    return 0;
}
