/**
 * \file spi.h
 * \brief Library for SPI communication
 * \author Martin Matysiak
 */

#ifndef SPI_H
    #define SPI_H

    /// The port at which the SPI Pins are
    #define SPI_PORT PORTB
    /// The direction register for the SPI Port
    #define SPI_PORT_DIR DDRB

    /// Chipselect Pin
    #define SPI_CS PB2
    /// MOSI Pin
    #define SPI_MOSI PB3
    /// MISO Pin
    #define SPI_MISO PB4
    /// SCK (CLK) Pin
    #define SPI_SCK PB5

    /// Macro to set the Chipselect (i.e. chipselect is pulled to low)
    #define SET_CS() SPI_PORT &= ~(1 << SPI_CS)
    /// Macro to clear the Chipselect (i.e. chipselect is pulled to high)
    #define CLEAR_CS() SPI_PORT |= (1 << SPI_CS)

    #include "global.h"

    /**
     * \brief Initializes the SPI Port.
     *
     * Configures the Pin directions and sets a basic configuration in the 
     * SPI Configuration register.
     */
    uint8_t spi_init();

    /**
     * \brief Sends a byte via SPI
     * \param pByte The byte to be sent
     */
    void spi_writeByte(uint8_t pByte);

    /**
     * \brief Reads a byte from the SPI and returns it
     * \return The byte which has been read
     */
    uint8_t spi_readByte();
    
    /**
     * \brief Switches SPI Configuration into Highspeed mode
     *
     * The speed will be set to the highest possible value, i.e.
     * F_SPI = F_CPU / 2
     */
    void spi_highspeed();
#endif
