/**
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
	    mail@k621.de
      www.k621.de

  Methoden zum Umgang mit dem ST22 GPS-Modul von Perthold Engineering.
  Klasse auf absolute Grundfunktionen reduziert. Datenausgabe erfolgt
  ungeparst als ASCII-String, keine Prüfung auf Validität o.ä..

  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#include "modules/gps.h"

uint8_t gps_init() {

  //UART-Schnittstelle initalisieren
  uart_init(UART_CONFIGURE(UART_ASYNC, UART_8BIT, UART_1STOP, UART_NOPAR), 
    UART_CALCULATE_BAUD(F_CPU, GPS_BAUDRATE));

  _delay_s(1);

  //Grundeinstellungen vornehmen
  unsigned char commands[8] = {
    0x01, // GGA 1Hz
    0x00, // no GSA
    0x00, // no GSV
    0x00, // no GLL
    0x01, // RMC 1Hz
    0x00, // no VTG
    0x00, // no ZDA
    0x00}; // in SRAM&FLASH

  gps_setParam(GPS_SET_NMEA, commands, 8);

  _delay_ms(50);

  unsigned char rate[2] = {
    0x01, // 1 Hertz
    0x00}; // In SRAM

  gps_setParam(GPS_SET_UPDATE_RATE, rate, 2);

  _delay_ms(50);

  unsigned char pps[2] = {
    0x01,  // 1PPS bei 3D Fix
    0x00}; // In SRAM

  gps_setParam(GPS_SET_1PPS, pps, 2);

  _delay_ms(50);

  return TRUE;
}

unsigned char gps_calculateCS(const unsigned char* pPayload, uint16_t pLength) {
  unsigned char checkSum = 0;
  for(uint8_t i = 0; i < pLength; i++) {
    checkSum ^= pPayload[i];
  }
  return checkSum;
}

unsigned char gps_setParam(unsigned char pCommand, unsigned char* pData, uint16_t pLength) {
  //Startsequenz senden (2 byte)
  uart_setChar(0xA0);
  uart_setChar(0xA1);
 
  //Payload Length senden (2 byte) == Length + 1 wegen Msg ID
  uart_setChar(((pLength+1) & 0xFF00) >> 8);
  uart_setChar((pLength+1) & 0x00FF);

  //Payload senden

  //MsgID (1 byte)
  uart_setChar(pCommand);

  //Data (pLength byte)
  for(uint8_t i = 0; i < pLength; i++) {
    uart_setChar(pData[i]);
  }

  //Checksum (1 byte)
  uart_setChar(gps_calculateCS(pData, pLength) ^ pCommand);

  //Stopsequenz senden (2 byte)
  uart_setChar(CR);
  uart_setChar(LF);

  return GPS_ACK;
}

uint8_t gps_getNMEA(char* pOutput, uint8_t pMaxLength) {
    // A dollar sign indicates the start of a NMEA sentence
    while(uart_getChar() != '$') {
        // burn energy
        _delay_ms(1);
    }
    
    // Copy data until LF
    uint8_t i = 0;
    pOutput[i++] = '$';

    char inChar;
    uint8_t commaCount = 0;

    do {
        while(!uart_hasData()) {
          // burn energy
        }
        
        inChar = uart_getChar();

        // count the commas, may be useful in the validity check afterwards
        if (inChar == ',') {
          commaCount++;
        }

        pOutput[i++] = inChar;
    } while((inChar != LF) && (i < (pMaxLength-1)));

    // Don't forget to terminate the string with a NUL character, otherwise 
    // everything might crash and burn
    pOutput[i] = 0;

    // Perform some validity checks and determine message type
    // TODO //

    // If we got here, no valid message pattern could be matched =(
    return GPS_NMEA_UNKOWN;
}
