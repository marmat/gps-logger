/**
 * \file spi.c
 * \brief Library for SPI communication
 * \author Martin Matysiak
 */

#include "protocols/spi.h"

uint8_t spi_init() {
    // configure port directions
    SPI_PORT_DIR |= (1 << SPI_SCK) | (1 << SPI_CS) | (1 << SPI_MOSI);
    SPI_PORT_DIR &= ~(1 << SPI_MISO);

    // pull-up on the MISO-line
    SPI_PORT |= (1 << SPI_MISO);

    CLEAR_CS();

    // wait a bit
    _delay_ms(10);

    // configure SPI register

    // SPI master mode, no interrupts (blocking mode)
    // MSB first
    // CPOL Mode 0 (CPOL=0, CPHA=0)
    // F_SPI = F_CPU / 128
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);

    return TRUE;
}

void spi_writeByte(uint8_t pByte) {
    SPDR = pByte;

    // Wait for transfer to complete
    while (!(SPSR & (1 << SPIF))) {
        // burn energy
    }
}

uint8_t spi_readByte() {
    // Send dummybyte in order to generate clock signals
    SPDR = 0xFF;  

    // Wait for transfer to complete
    while (!(SPSR & (1 << SPIF))) {
        // burn energy
    }

    return SPDR;
}

void spi_highspeed() {
    // Set speed in SPI Control Register to F_CPU / 4
    SPCR &=~ (1 << SPR1) | (1 << SPR0);

    // Double SPI Frequency (that makes F_SPI = F_CPU / 2)
    SPSR |= (1<<SPI2X); 
}
