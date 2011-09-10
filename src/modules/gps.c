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

uint8_t gps_init(uint8_t pFrequency, uint8_t pMessages) {

  //UART-Schnittstelle initalisieren
  uart_init(UART_CONFIGURE(UART_ASYNC, UART_8BIT, UART_1STOP, UART_NOPAR), 
    UART_CALCULATE_BAUD(F_CPU, GPS_BAUDRATE));

  _delay_s(1);

  //Grundeinstellungen vornehmen
  unsigned char commands[8] = {
    pMessages & GPS_NMEA_GGA ? 1 : 0, // GGA 1Hz
    pMessages & GPS_NMEA_GSA ? 1 : 0, // no GSA
    pMessages & GPS_NMEA_GSV ? 1 : 0, // no GSV
    pMessages & GPS_NMEA_GLL ? 1 : 0, // no GLL
    pMessages & GPS_NMEA_RMC ? 1 : 0, // RMC 1Hz
    pMessages & GPS_NMEA_VTG ? 1 : 0, // no VTG
    pMessages & GPS_NMEA_ZDA ? 1 : 0, // no ZDA
    0x00}; // in SRAM

  gps_setParam(GPS_SET_NMEA, commands, 8);

  _delay_ms(50);

  unsigned char rate[2] = {
    pFrequency, // pFrequency Hertz
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

uint8_t gps_isValidGGA(const char* pCommand, uint8_t pCommaCount) {
    // TODO //
    return GPS_NMEA_INVALID;
}

uint8_t gps_isValidRMC(const char* pCommand, uint8_t pCommaCount) {
    // TODO //
    return GPS_NMEA_INVALID;
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
    if (strStartsWith(pOutput, "$GPGGA")) {
        return GPS_NMEA_GGA | gps_isValidGGA(pOutput, commaCount);
    } else if (strStartsWith(pOutput, "$GPRMC")) {
        return GPS_NMEA_RMC | gps_isValidRMC(pOutput, commaCount);
    }

    // If we got here, no valid message pattern could be matched =(
    return GPS_NMEA_UNKOWN;
}
