/**
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
	    mail@k621.de
      www.k621.de

  Global benötigte Methoden und Strukturen

  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#ifndef GLOBAL_H
  #define GLOBAL_H

  #ifndef F_CPU
    ///Prozessorgeschwindigkeit = 7,3728MHz
    #define F_CPU 7372800UL
  #endif
  
  #define IO_PORT PORTC
  #define IO_CONF DDRC

  #define LED_STAT PC0

  #define LEDCODE_OFF() IO_PORT &= ~(1 << LED_STAT)
  #define LEDCODE_ON() IO_PORT |= (1 << LED_STAT)
  #define LEDCODE_BLINK() IO_PORT ^= (1 << LED_STAT)

  ///Begriff 'Wahr' für boolsche Ausdrücke
  #define TRUE 1
  ///Begriff 'Falsch' für boolsche Ausdrücke
  #define FALSE 0
  ///ASCII Zeichen Nr. 03 - End of Text
  #define ETX 0x03
  ///ASCII Zeichen Nr. 13 - Carriage Return (Wagenrücklauf)
  #define CR 0x0D
  ///ASCII Zeichen Nr. 10 - Linefeed (Zeilenumbruch)
  #define LF 0x0A

  #include <stdint.h>
  #include <stdlib.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <util/delay.h>

  void _delay_s(uint8_t pSeconds);
  
    /**
     * \brief Checks if the given string starts with the given pattern
     *
     * Both Strings have to be NULL-Terminated! pString will not be modified
     * in any way (in fact, it can't because of the const declaration).
     *
     * \param pString The string which should be checked
     * \param pPattern The pattern for which will be checked in pString
     * \return TRUE if the string starts with the pattern, FALSE otherwise
     */
    uint8_t strStartsWith(const char *pString, char *pPattern);
 #endif
