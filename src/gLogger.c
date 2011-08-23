/*
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
		mail@k621.de
		www.k621.de

	Version 1.20

  ########## Changes: ##########

  Version 1.1 (08.01.2010):
    * Bugfix: nofs_init scans the whole SD-card for 0x03 (ETX) instead of only
      the first sector -> Existing data won't be overwritten anymore (thanks to
      Ralf Fischer for detecting and solving the problem!)

  Version 1.2 (18.07.2011):
    * Bugfix: It seems that ETX wasn't written correctly. Because of this, the
      logger couldn't find a place to continue writing. This is fixed now

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
  
  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#include <avr/sleep.h>
#include "global.h"
#include "modules/nofs.h"
#include "modules/gps.h"

int main (void) {

	// Interrupts global aktivieren
  sei();  

	// Ports definieren
	IO_CONF |= (1 << LED_STAT);
	LEDCODE_ON();

  // Unnötige Module ausschalten (Power-reduction)
  PRR |= (1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRADC);

  _delay_s(1);

	// Module initalisieren
  if (!(nofs_init() && gps_init())) {
    while(1) {
      _delay_ms(100);
      LEDCODE_BLINK();    
    }
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
