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
 * \brief Indicates an error using the LED. Loops forever
 *
 * Once the error method is called, there is no way back. The code influences
 * the flashing sequence of the LED. In general, the LED flashes (code+1) times 
 * with a break of 100ms between each flash, then the LED is 500ms off and then 
 * the whole sequence repeats itself.
 *
 * \param code The error code which shall be displayed
 */
void error(uint8_t code) {
    LEDCODE_OFF();
    
    while (TRUE) {
        for (uint8_t i = 0; i <= code; i++) {
            LEDCODE_ON();
            _delay_ms(100);
            LEDCODE_OFF();
            _delay_ms(100);
        }
        
        _delay_ms(250);
        _delay_ms(150);
    }
}

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

    _delay_ms(250);

    // Initialize each module seperately in order to display a better error code
    if (!sdmmc_init()) {
        error(1);
    }
  
    if (!nofs_init()) {
        error(2);
    }
    
    if (!gps_init(1, GPS_NMEA_GGA | GPS_NMEA_RMC)) {
        error(3);
    }

    // Write a short information string containing the firmware version (NMEA compliant)
    nofs_writeString("$PGLGVER,1.4a2\r\n");

    LEDCODE_OFF();

    char nmeaBuf[128];
    uint8_t result = 0;

    while(1) {
        // We'll write the data only if it contains a valid position
        if (gps_getNMEA(nmeaBuf, 128) & GPS_NMEA_VALID) {
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
