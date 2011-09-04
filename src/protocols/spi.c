/**
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
	    mail@k621.de
      www.k621.de

  Befehlsbibliothek für das SPI-Protkoll

  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#include "protocols/spi.h"

uint8_t spi_init() {
  
  // SPI-Port konfigurieren:
  //  SCK, CS, MOSI als Ausgang
  //  MISO als Eingang, mit aktiviertem Pull-Up
  //  CS Standardmäßig High

  //  Portrichtungen definieren
  SPI_PORT_DIR |= (1 << SPI_SCK) | (1 << SPI_CS) | (1 << SPI_MOSI);
  SPI_PORT_DIR &= ~(1 << SPI_MISO);

  // Pull-Up auf MISO Leitung
  SPI_PORT |= (1 << SPI_MISO);

  // Chipselect löschen
  CLEAR_CS();
  
  // Kurz warten
  _delay_ms(10);

  // SPI-Register konfigurieren

  // SPI Master Mode, ohne Interrupts (Blocking mode)
  // MSB Zuerst
  // CPOL Mode 0 (CPOL=0, CPHA=0)
  // F_SPI = F_CPU / 4
  SPCR = (1 << SPE) | (1 << MSTR);

  // Double SPI Frequency (that makes F_SPI = F_CPU / 2)
  SPSR |= (1<<SPI2X); 

  return TRUE;
}

void spi_writeByte(uint8_t pByte) {
  SPDR = pByte;

  // Auf Transferergebnis warten
  while (!(SPSR & (1 << SPIF))) {
    // Energie verbrennen
  }
}

uint8_t spi_readByte() {
  // Dummybyte "senden", damit SCK-Impulse generiert werden
  SPDR = 0xFF;  

  // Auf Transferergebnis warten
  while (!(SPSR & (1 << SPIF))) {
    // Energie verbrennen
  }
    
  return SPDR;
}
