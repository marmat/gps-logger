/**
 * \file uart.c
 * \brief Library for UART communication
 * \author Martin Matysiak
 */

#include "protocols/uart.h"
#include <avr/interrupt.h>

/// FIFO input buffer
static volatile char uart_inputBuf0[UART_INPUT_BUFFER_SIZE];
/// Index of the last character that has been read in the input buffer
static volatile uint8_t uart_inputBuf0Read = 0;
/// Index of the last charachter that has been written in the input buffer
static volatile uint8_t uart_inputBuf0Write = 0;

/// FIFO output buffer
static volatile char uart_outputBuf0[UART_OUTPUT_BUFFER_SIZE];
/// Index of the last character that has been read in the output buffer
static volatile uint8_t uart_outputBuf0Read = 0;
/// Index of the last character that has been written in the output buffer
static volatile uint8_t uart_outputBuf0Write = 0;

void uart_init(uint8_t pConfig, uint16_t pUbr) {
    // write baudrate config (high-byte has to be written first!)
    UBRR0H = (uint8_t)(pUbr >> 8);
    UBRR0L = (uint8_t)pUbr;

    // configure port and activate interrupts
    UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

    // write frame configuration
    UCSR0C = pConfig;
}

void uart_changeBaud(uint16_t pUbr) {
    // TODO
}

unsigned char uart_getChar() {
    if (uart_inputBuf0Read != uart_inputBuf0Write) {
        // increment reading pointer while catching a possible array overflow
        if (++uart_inputBuf0Read >= UART_INPUT_BUFFER_SIZE) {
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
    // write byte into the output buffer, wait if the buffer is currently full
    if (uart_outputBuf0Write+1 >= UART_OUTPUT_BUFFER_SIZE) {
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
    // simply call setChar for each character in the string
    uint8_t i = 0;
    while(pData[i]) {
        uart_setChar(pData[i++]);
    }
}

void uart_clearBuf() {
    uart_inputBuf0Read = uart_inputBuf0Write;
}

/**
 * \brief Interrupt handling for incoming UART-data
 * 
 * The method will write the incoming character directly into the input buffer.
 * If the buffer is full, characters may be discarded.
 */
ISR(USART_RX_vect) {
    if(uart_inputBuf0Write+1 >= UART_INPUT_BUFFER_SIZE) {
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

/**
 * \brief Interrupt handling for outgoing UART-data
 *
 * As long as there is data in the output buffer, the method will write the data
 * into the specific UART register. When the buffer is empty, the interrupt will
 * deactivate itself.
 */
ISR(USART_UDRE_vect) {
    // write next byte until reading index == writing index
    if (uart_outputBuf0Read != uart_outputBuf0Write) {
        if (++uart_outputBuf0Read >= UART_OUTPUT_BUFFER_SIZE) {
            uart_outputBuf0Read = 0;
        }

        UDR0 = uart_outputBuf0[uart_outputBuf0Read];
    } else {
        // buffer empty, deactivate interrupt
        UCSR0B &= ~(1 << UDRIE0);
    }
}
