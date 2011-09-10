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

#ifndef GPS_H
  #define GPS_H

  #include "global.h"
  #include "protocols/uart.h"

  #define GPS_ACK 0x83
  #define GPS_NACK 0x84

  #define GPS_GET_UPDATE_RATE 0x10
  #define GPS_GET_DATE 0x2D
  #define GPS_GET_1PPS 0x3F

  #define GPS_SET_NMEA 0x08
  #define GPS_SET_POWER 0x0C
  #define GPS_SET_UPDATE_RATE 0x0E
  #define GPS_SET_1PPS 0x3E
   
  // Indicators if NMEA Message is valid or invalid (validity differs for each Message type)
  #define GPS_NMEA_VALID 0x01
  #define GPS_NMEA_INVALID 0x00

  // Possible NMEA-Messages that may be returned
  #define GPS_NMEA_GGA 0x02
  #define GPS_NMEA_RMC 0x04
  /// others not used yet
  #define GPS_NMEA_UNKOWN 0x80
  
  /// A bitmask to extract the message type from a getNMEA return value
  #define GPS_NMEA_TYPEMASK 0xFE

  /// BAUD-Rate für die serielle Schnittstelle zum GPS-Modul
  #define GPS_BAUDRATE 9600UL

  ///Initalisiert das GPS-Modul
  uint8_t gps_init();

  /** 
    Setzt einen Parameter des GPS-Moduls mit den gegebenen Daten

    \param pCommand Der zu setzende Parameter (siehe Konstanten)
    \param pData Die Daten, die für den Parameter geschrieben werden sollen
    \return GPS_ACK bei Erfolg, ansonsten GPS_NACk
  */
  unsigned char gps_setParam(unsigned char pCommand, unsigned char* pData, uint16_t pLength);
  
  /**
   * \brief Checks if the given message is a valid $GPGGA sentence
   * 
   * In this case, validity incorporates a correct checksum, a correct comma
   * count and a valid GPS fix.
   *
   * \param pCommand The NMEA-sentence
   * \param pCommaCount The number of commas in the sentence (tbd if necessary)
   * \return GPS_VALID if sentence is a valid $GPGGA sentence, GPS_INVALID
   * otherwise
   */
  uint8_t gps_isValidGGA(const char* pCommand, uint8_t pCommaCount);
  
  /**
   * \brief Checks if the given message is a valid $GPRMC sentence
   * 
   * In this case, validity incorporates a correct checksum, a correct comma
   * count and a valid GPS fix.
   *
   * \param pCommand The NMEA-sentence
   * \param pCommaCount The number of commas in the sentence (tbd if necessary)
   * \return GPS_VALID if sentence is a valid $GPRMC sentence, GPS_INVALID
   * otherwise
   */
  uint8_t gps_isValidRMC(const char* pCommand, uint8_t pCommaCount);

  /**
    \brief Writes an NMEA-String into the given output buffer and returns its type
    
    \param pOutput The buffer in which the NMEA string shall be written
    \param pMaxLength The buffer's maximal length
    \return A byte composed of the message type (bits 1-7) and a bit indicating
    if the message is valid or not (bit 0). 
  */
  uint8_t gps_getNMEA(char* pOutput, uint8_t pMaxLength);
#endif
