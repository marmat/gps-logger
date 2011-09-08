/*
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
		mail@martin-matysiak.de
		www.martin-matysiak.de

	Version 1.30

  ########## Changes: ##########

  Version 1.1 (08.01.2010):
    * Bugfix: nofs_init scans the whole SD-card for 0x03 (ETX) instead of only
      the first sector -> Existing data won't be overwritten anymore (thanks to
      Ralf Fischer for detecting and solving the problem!)

  Version 1.2 (18.07.2011):
    * Bugfix: It seems that ETX wasn't written correctly. Because of this, the
      logger couldn't find a place to continue writing. This is fixed now
      
  Version 1.3 (29.08.2011):
    * Bugfix: GPS-Module was set to 2Hz despite of comment saying it was 1Hz,
      now it's _really_ set to 1Hz
    * Bugfix: The UART library produced regularly buffer overruns which weren't
      detected due to the 2Hz bug. This won't happen anymore (thanks to Bastian
      Wegener!)

  ########## Änderungen: ##########

  Version 1.1 (08.01.2010):
    * Bugfix: nofs_init durchsucht nun die gesamte SD-Karte nach 0x03 (ETX) an-
      stelle nur des ersten Sektors -> Bereits vorhandene Aufnahmen werden nicht
      mehr überschrieben (Vielen Dank an Ralf Fischer für Erkennung und Lösung
      des Problems!)
      
  Version 1.2 (18.07.2011):
    * Bugfix: Anscheinend wurde das ETX byte nicht ordentlich geschrieben. Des-
      wegen konnte der Logger keine Position zum Fortsetzen der Aufzeichnung
      finden. Das ist nun behoben
      
  Version 1.3 (29.08.2011):
    * Bugfix: Das GPS-Modul war, entgegen des Kommentars, auf 2Hz gestellt. Nun
      ist es _wirklich_ auf 1Hz gestellt.
    * Bugfix: Die UART Bibliothek produzierte regelmäßig Buffer Überläufe, die
      wegen des 2Hz-Bugs nicht/kaum bemerkt wurden. Dies ist nun ebenfalls be-
      hoben (vielen dank an Bastian Wegener!)
  
  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#include <avr/sleep.h>
#include "global.h"
#include "modules/nofs.h"
#include "modules/gps.h"

/**
 * \brief Indicates an error using the LED. Loops forever
 *
 * Once the error method is called, there is no way back. The code influences
 * the flashing sequence of the LED. In general, the LED flashes code times with
 * a break of 100ms between each flash, then the LED is 500ms off and then 
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

int main (void) {

	// Interrupts global aktivieren
  sei();  

	// Ports definieren
	IO_CONF |= (1 << LED_STAT);
	LEDCODE_ON();

  // Unnötige Module ausschalten (Power-reduction)
  PRR |= (1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRADC);

  _delay_s(1);

    // Initialize each module seperately in order to display a better error code
    while (!sdmmc_init()) {
        error(1);
    }
    
    if (!nofs_init()) {
        error(2);
    }
    
    if (!gps_init()) {
        error(3);
    }

  LEDCODE_OFF();

  char nmeaBuf[128];

  while(1) {
    // Geschrieben wird nur, wenn NMEA-Kommando einen Fix hatte
    if(gps_getNmeaSentence(nmeaBuf, 128)) {
      nofs_writeString(nmeaBuf);
    }

    // Blinken !!! (das Wichtigste in diesem ganzen Code)
    LEDCODE_BLINK();
    _delay_ms(30);
    LEDCODE_BLINK(); 
    sleep_mode();

  }
 
	// Wird nie erreicht
	return 0;
}
