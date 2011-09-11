/**
 * \file uart.h
 * \brief Library for UART communication
 * \author Martin Matysiak
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

    /// Size of the input buffer in bytes
    #define UART_INPUT_BUFFER_SIZE 128
    
    /// Size of the output buffer in bytes
    #define UART_OUTPUT_BUFFER_SIZE 32

    /// Macro which generates a configuration byte for the UART register
    #define UART_CONFIGURE(pMode, pBits, pStop, pParity) pMode | pBits | pStop | pParity

    /// Macro which calculates the correct value for the baudrate register
    #define UART_CALCULATE_BAUD(pFrequency, pBaudrate) (pFrequency / (16 * pBaudrate)) - 1

    /// Macro for sending a "new line"-sequence
    #define UART_NEWLINE() uart_setChar(CR); uart_setChar(LF);

    /**
     * \brief Initializes the UART-port
     * 
     * \param pConfig The UART configuration byte
     * \param pUbr A 16-bit integer containing the baudrate configuration value
     */
    void uart_init(uint8_t pConfig, uint16_t pUbr);

    /**
     * \brief Takes a character from the input buffer and returns it (FIFO).
     * \return The first not yet processed byte
     */
    unsigned char uart_getChar();

    /**
     * \brief Takes a string from the input buffer and writes it into pResult.
     * 
     * The string has at most pResultSize - 1 characters. It may have less if
     * a NULL-byte is encountered before. Then the string will be terminated at
     * this point.
     * 
     * \param pResult A pointer to the output array
     * \param pResultSize An integer containing the size of the output array
     * \return The count of actually returned characters
     */
    uint8_t uart_getString(char* pResult, uint8_t pResultSize);

    /**
     * \brief Checks if there is unprocessed data in the input buffer.
     *
     * This method can be used in order to prevent a blockage of the cpu
     * when trying to read from an empty input buffer.
     * 
     * \return TRUE if data is available, otherwise FALSE
     */
    uint8_t uart_hasData();

    /**
     * \brief Sends a character.
     * \param pData The character which shall be sent
     */
    void uart_setChar(char pData);

    /**
     * \brief Sends a string.
     * \param pData A pointer to a NULL-terminated character-array with data
     * which shall be sent
     */
    void uart_setString(const char* pData);

    /**
     * \brief Empties the input buffer, discarding everything inside.
     */ 
    void uart_clearBuf();
#endif
