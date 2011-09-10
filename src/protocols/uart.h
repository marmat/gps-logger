/**
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
	    mail@k621.de
      www.k621.de

  Implementierung des UART-Protokolls zum Austausch von Daten

  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#ifndef UART_H
  #define UART_H

  #include "global.h"

  #define UART_ASYNC 0x00
  #define UART_SYNC 0x40
  #define UART_SPI 0xC0

  #define UART_5BIT 0x00
  #define UART_6BIT 0x02
  #define UART_7BIT 0x04
  #define UART_8BIT 0x06

  #define UART_1STOP 0x00
  #define UART_2STOP 0x08

  #define UART_NOPAR 0x00
  #define UART_EVEN 0x20
  #define UART_ODD 0x30

  #define UART_0 0
  #define UART_1 1

  #define UART_NOERROR 0x00
  #define UART_BUF_OVERRUN 0x02

  #define UART_INPUT_BUFFER_SIZE 128
  #define UART_OUTPUT_BUFFER_SIZE 32

  #define UART_BUFFER_OVERFLOW 0x00

  ///Makro zum erstellen des Konfigurationsbytes für UART-Register C
  #define UART_CONFIGURE(pMode, pBits, pStop, pParity) pMode | pBits | pStop | pParity

  ///Makro zum Berechnen des Wertes für das Baudraten-Register
  #define UART_CALCULATE_BAUD(pFrequency, pBaudrate) (pFrequency / (16 * pBaudrate)) - 1

  ///Makro zum senden eines Zeilenumbruchs (Zeichenkombination <CR><LF>)
  #define UART_NEWLINE() uart_setChar(CR); uart_setChar(LF);

  /** 
    Initalisiert den gegebenen UART-Port.

    \param pConfig Das Konfigurationsbyte für Register C
    \param pUbr Der Wert für das Baudraten-Register
  */
  void uart_init(uint8_t pConfig, uint16_t pUbr);

  /**
    Ruft ein Zeichen aus dem Pufferspeicher ab und gibt ihn zurück

    \return Das erste noch nicht empfangene Byte
  */
  unsigned char uart_getChar();

  /**
    Gibt eine Zeichenkette zurück die im Puffer gespeichert ist. Die
    Länge der Zeichenkette ist gleich der Anzahl der Zeichen bis zum
    ersten \\0-Zeichen, höchstens aber pResultSize - 1 (auch wenn die
    Zeichenkette noch nicht abgeschlossen war).

    \param pResult Ein Zeiger auf den Ergebnis-Speicher
    \param pResultSize Die Größe des Ergebnis-Speichers
    \return Die Anzahl der tatsächlich empfangenen Bytes
  */
  uint8_t uart_getString(char* pResult, uint8_t pResultSize);

  /**
    Überprüft für den gegebenen Port, ob sich Daten im Puffer befinden.
    Dadurch soll einem Blockieren des µC beim nicht-vorhandensein von
    Daten entgegengewirkt werden.

    \return TRUE falls Daten vorhanden, ansonsten FALSE
  */
  uint8_t uart_hasData();

  /**
    Sendet ein Zeichen an den gegebenen Port.

    \param pData Das Zeichen, das gesendet werden soll
  */
  void uart_setChar(char pData);

  /**
    Sendet eine Zeichenkette an den gegebenen Port.

    \param pData Ein Zeiger auf die Zeichenkette
  */
  void uart_setString(const char* pData);

  /**
    Leert den Lesebuffer des gegebenen Ports
  */ 
  void uart_clearBuf();

#endif
