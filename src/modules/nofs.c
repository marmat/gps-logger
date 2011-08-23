/**
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
	    mail@k621.de
      www.k621.de

  NoFS - NoFileSystem - Eine Art, das Speichermedium im RAW-Modus
  zu beschreiben, mit automatischer Erkennung der letzten Schreib-
  position, um Datenverlust zu verhindern. Nach Außen sind nur
  Schreib-Methoden verfügbar, es handelt sich also um eine Art
  "WOM" (Write-Only-Memory).

  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#include "modules/nofs.h"

// Lokale Variablen
uint32_t fCurrentSector = 0; // Nummer das aktuell geöffneten Sektors
uint16_t fCurrentByte = 0; // Verharrt immer auf der Arrayposition des aktuellen ETX
uint8_t fWriteCount = 0; // Wird bei jedem "in-den-Buffer" Schreibvorgang hochgezählt

char sectorBuf[NOFS_BUFFER_SIZE];

uint8_t nofs_init() {
  if (sdmmc_init()) {
    
    do {
      // Schreibposition (== ETX, 0x03) auf SD-Karte suchen
      sdmmc_readSector(fCurrentSector++, sectorBuf, NOFS_BUFFER_SIZE);

      for(fCurrentByte = 0; fCurrentByte < NOFS_BUFFER_SIZE; fCurrentByte++) {
        if (sectorBuf[fCurrentByte] == ETX) {
          break;
        }
      }
	
    // Solange der Byte-Zeiger an das Ende des Sektors gelangt ist und das letzte Zeichen
    // kein ETX ist, wird die Schleife ein weiteres mal durchlaufen und der nächste Sek-
    // tor durchsucht.
    } while((fCurrentByte == NOFS_BUFFER_SIZE) && (sectorBuf[fCurrentByte - 1] != ETX));

    // Das letzte "++" rückgängig machen
    fCurrentSector--;

    return TRUE;
  }

  return FALSE;
}

uint8_t nofs_writeString(char* pString) {
  // Daten in Buffer kopieren, wenn Buffer voll ist, auf SD schreiben
  // und nächsten Buffer anlegen.
  uint8_t i = 0;

  while (pString[i]) {
    if(fCurrentByte < NOFS_BUFFER_SIZE) {
      // Zeichenweise in den Sektor kopieren
      sectorBuf[fCurrentByte++] = pString[i++];
    } else {    
      // Sektor voll, auf SD schreiben und nächsten holen
      sdmmc_writeSector(fCurrentSector++, sectorBuf, NOFS_BUFFER_SIZE);
      _delay_ms(10);

      sdmmc_readSector(fCurrentSector, sectorBuf, NOFS_BUFFER_SIZE);
      
      // Sicherheitshalber ein Endbyte (ETX) in den nächsten Sektor schreiben
      sectorBuf[0] = ETX;
      sdmmc_writeSector(fCurrentSector, sectorBuf, NOFS_BUFFER_SIZE);
      
      fCurrentByte = 0;
    }
  }

  // Letztes Byte als ETX setzen und bisherigen Block schreiben
  sectorBuf[fCurrentByte] = ETX;

  // Buffer schreiben wenn der n-te Befehl ausgeführt wird
  if (fWriteCount++ == NOFS_WRITE_BUFFER_AFTER_NTH_COMMAND) {
    sdmmc_writeSector(fCurrentSector, sectorBuf, NOFS_BUFFER_SIZE);
    fWriteCount = 0;
  }

  return TRUE;
}
