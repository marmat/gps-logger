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

uint8_t gps_checkNMEA(const char* pSentence, uint8_t pMessageType, char* pPrefix, 
    uint8_t pValidityToken, char* pValidityCheck, uint8_t pCheckEquality) {

    // Sweep through the sentence once, while doing:
    // * Prefix check (return instantly if invalid)
    // * Searching for validityToken and comparison with validityCheck
    // * Calculation of checksum and comparison with given one (if given)
    // WARNING: Long and ugly code ahead ;-)
    
    // Counter variables for the different strings which have to be checked
    uint8_t sentenceCounter = 0;
    uint8_t prefixCounter = 0;
    uint8_t validityCounter = 0;
    
    // Counter to count the current token (only used when validityToken is set)
    uint8_t tokenCounter = 0;
    
    // Variable which will contain the validity state of the sentence, defaults
    // to VALID
    uint8_t messageValidity = GPS_NMEA_VALID;
    
    // Checksum is calculated by XORing every character between the '$' and '*'
    // We set its initial value to '$' so that the loop does not have to 
    // differentiate between the first and other characters (initial '$' xor
    // first character '$' == 0).
    char checksum = '$';
    
    do {
        // Prefix check is done if the prefixCounter points to the end of the
        // given prefix
        if (pPrefix[prefixCounter] != '\0') {
            // return instantl if the prefixes do not match
            if (pSentence[sentenceCounter] != pPrefix[prefixCounter]) {
                return GPS_NMEA_UNKNOWN;
            }
            
            prefixCounter++;
        }
        
        // Calculate checksum
        checksum ^= pSentence[sentenceCounter];
        
        // Perform a validity check only if a validityToken was specified
        if (pValidityToken) {
            if (pSentence[sentenceCounter] == ',') {
                tokenCounter++;
            } else { // check non-comma characters only
                if (tokenCounter == pValidityToken) {
                    // Catch if token is too long
                    if (pValidityCheck[validityCounter] == '\0') {
                        messageValidity = GPS_NMEA_INVALID;
                    } else {
                        // Check if tokens differ
                        if (pSentence[sentenceCounter] != pValidityCheck[validityCounter++]) {
                            messageValidity = GPS_NMEA_INVALID;   
                        }
                    }
                }
            }
        }
    } while (pSentence[++sentenceCounter] != '\0');
    
    
    // If a validityToken was specified, perform some final checks
    if (pValidityToken != 0) {
        // Check if token was too short (i.e. validityCounter still points to a
        // not-null character)
        if (pValidityCheck[validityCounter] != '\0') {
            messageValidity = GPS_NMEA_INVALID;
        }
            
        // If a validityToken was specified, messageValidity currently holds _VALID
        // if the token was equal to the given one and _INVALID otherwise. Check, if
        // checkEquality was set to FALSE. In this case, negate the messageValidity
        // variable
        if (pCheckEquality == FALSE) {
            if (messageValidity == GPS_NMEA_VALID) {
                messageValidity = GPS_NMEA_INVALID;
            } else {
                messageValidity = GPS_NMEA_VALID;
            }
        }
    }
    
    // If a checksum was given, xor the last three characters again (to remove
    // them from our calculation) and compare the calculation to the given
    // checksum
    if (pSentence[sentenceCounter - 3] == '*') {
        checksum = checksum ^ '*' ^ pSentence[sentenceCounter - 2] ^ pSentence[sentenceCounter - 1];
        
        // convert hex-string to uint8_t
        uint8_t givenChecksum = (hexCharToInt(pSentence[sentenceCounter - 2]) << 4)
            + hexCharToInt(pSentence[sentenceCounter - 1]);
        
        // on checksum mismatch, the message is not only invalid, but completely 
        // corrupt, therefore GPS_NMEA_UNKNOWN will be returned.
        if (checksum != givenChecksum) {
            return GPS_NMEA_UNKNOWN;
        }
    }
    
    return pMessageType | messageValidity;
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

    do {
        while(!uart_hasData()) {
          // burn energy
        }
        
        inChar = uart_getChar();

        pOutput[i++] = inChar;
    } while((inChar != LF) && (i < (pMaxLength-1)));

    // Don't forget to terminate the string with a NUL character, otherwise 
    // everything might crash and burn
    pOutput[i] = 0;

    // Determine the correct validity checker by checking characters 3-5 in a
    // "Trie"-like if-sentence tree, saves many cycles in comparison to full
    // prefix check for each possible message
    switch (pOutput[3]) {
        case 'G': //GGA, GSA, GSV or GLL
            switch (pOutput[4]) {
                case 'G': //only GGA left
                    return gps_checkNMEA(pOutput, GPS_NMEA_GGA, "$GPGGA", 6, "0", FALSE);
                case 'S': //GSA or GSV:
                    if (pOutput[5] == 'A') {
                        return gps_checkNMEA(pOutput, GPS_NMEA_GSA, "$GPGSA", 2, "1", FALSE);
                    } else {
                        return gps_checkNMEA(pOutput, GPS_NMEA_GSV, "$GPGSV", 0, "", FALSE);
                    }
                case 'L': //only GLL left
                    return gps_checkNMEA(pOutput, GPS_NMEA_GLL, "$GPGLL", 6, "A", TRUE);
                default:
                    return GPS_NMEA_UNKNOWN;
            } //btw: no break;s are necessary as every case returns sth.
        case 'R': //only RMC left
            return gps_checkNMEA(pOutput, GPS_NMEA_RMC, "$GPRMC", 2, "A", TRUE);
        case 'V': //only VTG left
            return gps_checkNMEA(pOutput, GPS_NMEA_VTG, "$GPVTG", 9, "N", FALSE);
        case 'Z': //only ZDA left
            return gps_checkNMEA(pOutput, GPS_NMEA_ZDA, "$GPZDA", 0, "", FALSE);
        default:
            return GPS_NMEA_UNKNOWN;
    }
}
