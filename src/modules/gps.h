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

  ///BAUD-Rate für die serielle Schnittstelle zum GPS-Modul
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
    Wartet auf einen vollständiges NMEA-Kommando und schreibt dieses in den Buffer

    \param pOutput Der Buffer, in den das NMEA-Kommando geschrieben werden soll
    \param pMaxLenght Die Länge des Buffers
    \return TRUE wenn Positionsmeldung einen Fix hatte, ansonsten FALSE
  */
  uint8_t gps_getNmeaSentence(char* pOutput, uint8_t pMaxLength);
#endif
