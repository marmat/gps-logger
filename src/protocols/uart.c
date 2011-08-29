/**
  ########## gLogger Mini ##########
    
	Von Martin Matysiak
	    mail@k621.de
      www.k621.de

  Implementierung des UART-Protokolls zum Austausch von Daten

  ########## Licensing ##########

  Please take a look at the LICENSE file for detailed information.
*/

#include "protocols/uart.h"
#include <avr/interrupt.h>

//FIFO-Buffer
static volatile char uart_inputBuf0[UART_BUFFER_SIZE];
static volatile uint8_t uart_inputBuf0Read = 0;
static volatile uint8_t uart_inputBuf0Write = 0;

static volatile char uart_outputBuf0[UART_BUFFER_SIZE];
static volatile uint8_t uart_outputBuf0Read = 0;
static volatile uint8_t uart_outputBuf0Write = 0;

void uart_init(uint8_t pConfig, uint16_t pUbr) {
  //Baudrate schreiben (High muss zuerst gesetzt werden!)
  UBRR0H = (uint8_t)(pUbr >> 8);
  UBRR0L = (uint8_t)pUbr;

  //Port und Interrupt aktivieren
  UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

  //Frame-Konfiguration schreiben
  UCSR0C = pConfig;
}

unsigned char uart_getChar() {
  if (uart_inputBuf0Read != uart_inputBuf0Write) {
    // Increment reading pointer while catching a possible array overflow
    if (++uart_inputBuf0Read >= UART_BUFFER_SIZE) {
      uart_inputBuf0Read = 0;
    }
  
    return uart_inputBuf0[uart_inputBuf0Read];
  } 

  return '\0';
}

uint8_t uart_hasData() {
  return uart_inputBuf0Read != uart_inputBuf0Write;
}

uint8_t uart_getString(char* pResult, uint8_t pResultSize) {
  uint8_t currentChar = 0;
  while(currentChar < pResultSize) {
    pResult[currentChar++] = uart_getChar();
    if ((!uart_hasData()) || (pResult[currentChar - 1] == LF)) {
      break;
    }
  }

  pResult[currentChar-1] = '\0';
  if((currentChar > 1) && (pResult[currentChar-2] == CR)) {
    pResult[currentChar-2] = '\0';
  }
  
  return currentChar;
}

void uart_setChar(char pData) {
  //Byte in den Sendebuffer schreiben, Sendeinterrupt aktivieren
  //Schreibzeiger um Eins vorrücken, wenn dann auf Lesezeiger liegt, warten
  if (uart_outputBuf0Write+1 >= UART_BUFFER_SIZE) {
    // writing pointer is at the end of the buffer array, next index will be 0  
    while (uart_outputBuf0Read == 0) {
      // wait, buffer is full
    }
    
    uart_outputBuf0Write = 0;
  } else {
    while (uart_outputBuf0Write+1 == uart_outputBuf0Read) {
      // wait, buffer is full
    }
    
    uart_outputBuf0Write++;
  }

  // write character into buffer   
  uart_outputBuf0[uart_outputBuf0Write] = pData;

  // activate interrupt
  UCSR0B |= (1 << UDRIE0); 
}

void uart_setString(const char* pData) {
  uint8_t i = 0;
  while(pData[i]) {
    uart_setChar(pData[i++]);
  }
}

void uart_clearBuf() {
  uart_inputBuf0Read = uart_inputBuf0Write;
}

//Interrupt-Abarbeitung
ISR(USART_RX_vect) {
  if(uart_inputBuf0Write+1 >= UART_BUFFER_SIZE) {
    // writing pointer is at the end of the buffer array, next index will be 0
    if(uart_inputBuf0Read != 0) {
        uart_inputBuf0Write = 0;
        uart_inputBuf0[uart_inputBuf0Write] = UDR0;
        return;
    }
  } else {
    if(uart_inputBuf0Write+1 != uart_inputBuf0Read) {
      uart_inputBuf0[++uart_inputBuf0Write] = UDR0;
      return;
    }
  }
  
  // if the method didn't return, it means that the buffer is full
  // discard the byte in order to prevent a blocked UDR register
  char garbage;
  garbage = UDR0;
}

ISR(USART_UDRE_vect) {
  //Solange nächstes Byte schreiben, bis Lese == Schreibposition
  if (uart_outputBuf0Read != uart_outputBuf0Write) {
    if (++uart_outputBuf0Read >= UART_BUFFER_SIZE) {
      uart_outputBuf0Read = 0;
    }
  
    UDR0 = uart_outputBuf0[uart_outputBuf0Read];
  } else {
    //Buffer abgearbeitet, Interrupt deaktivieren
    UCSR0B &= ~(1 << UDRIE0);
  }
}
