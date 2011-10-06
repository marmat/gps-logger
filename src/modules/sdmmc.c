/**
 * \file sdmmc.c
 * \brief Library for handling SD/MMC memory cards
 * \author Martin Matysiak
 */

#include "modules/sdmmc.h"

/// The block length which is currently set
uint16_t fBlockLength = SDMMC_SECTOR_SIZE;

void sdmmc_init() {
    // Initializes SPI interface first
    if (!spi_init()) {
        error(ERROR_SDMMC);
    }

    uint8_t response = 0;
    uint8_t retry = 0;

    // Send some dummy bytes before actual data
    for(uint8_t i = 0; i < 20; i++) {
        spi_writeByte(0xFF);
    }

    // Send command 0 (GO_IDLE_STATE, i.e. change from SD into SPI mode)
    while (response != 1) {
        response = sdmmc_writeCommand(SDMMC_GO_IDLE_STATE, 0, SDMMC_GO_IDLE_STATE_CRC); // 0x95 is a precalculated CRC checksum
        
        // If the device does not respond correctly, return FALSE after
        // having it tried several times
        if (retry++ == 0xFF) {
            CLEAR_CS();
            error(ERROR_SDMMC);
        }
    }

    // Send copmmand 1 (SEND_OP_COND, initialize card)
    response = 0xFF;
    retry = 0;

    while (response != 0) {
        response = sdmmc_writeCommand(SDMMC_SEND_OP_COND, 0, SDMMC_DEFAULT_CRC);

        if (retry++ == 0xFF) {
            CLEAR_CS();
            error(ERROR_SDMMC);
        }
    }

    CLEAR_CS();

    // switch to higher SPI frequency once initialized
    spi_highspeed();
}

uint8_t sdmmc_writeCommand(uint8_t pCommand, uint32_t pArgument, uint8_t pCrc) {
    // The command is composed out of 6 Byte:
    // Byte 1: 0b01xxxxxx where xx = pCommand
    // Byte 2-5: pArgument
    // Byte 6: 0byyyyyyy1 where yy = CRC7 (ignored when card is in SPI mode)

    // Make sure that bits 7 and 6 of pCommand are 0 and 1
    pCommand &= ~(1 << 7);
    pCommand |= (1 << 6);

    // Calculate CRC7 (// TODO //, currently precalculated values)

    // Bit 0 of the checksum byte has to be set
    pCrc |= 1;

    // Send some dummy clock signals
    CLEAR_CS();
    spi_writeByte(0xFF);
    SET_CS();

    // Send the data
    spi_writeByte(pCommand);
    spi_writeByte((uint8_t)((pArgument & 0xFF000000) >> 24));
    spi_writeByte((uint8_t)((pArgument & 0x00FF0000) >> 16));
    spi_writeByte((uint8_t)((pArgument & 0x0000FF00) >> 8));
    spi_writeByte((uint8_t)(pArgument & 0x000000FF));
    spi_writeByte(pCrc);

    // Get a response
    uint8_t result = 0xFF;
    uint8_t retry = 0;

    while(result == 0xFF) {
        result = spi_readByte();
        if (retry++ == 0xFF) {
            result = 0;
            break;
        }
    }

    return result;
}

uint8_t sdmmc_writeSector(uint32_t pSectorNum, char* pInput) {
    SET_CS();

    // Send command 24 (WRITE_BLOCK)
    if (sdmmc_writeCommand(SDMMC_WRITE_BLOCK, SECTOR_TO_BYTE(pSectorNum), SDMMC_DEFAULT_CRC) != 0) {
        CLEAR_CS();
        return FALSE;
    }

    // Send some dummy clocks
    for(uint8_t i = 0; i < 100; i++) {
        spi_readByte();
    }

    // Send start-byte
    spi_writeByte(0xFE);

    // Send data
    for(uint16_t i = 0; i < fBlockLength; i++) {
        spi_writeByte(pInput[i]);
    }

    // Send dummy CRC checksum (won't be checked in SPI mode)
    spi_writeByte(0xFF);
    spi_writeByte(0xFF);

    // Get response
    if ((spi_readByte() & 0x1F) != 0x05) {
        CLEAR_CS();
        return FALSE;
    }

    // Wait until memory card is ready for new commands
    while (spi_readByte() != 0xFF) {
        // burn energy
    }

    CLEAR_CS();
    return TRUE;
}

uint8_t sdmmc_readSector(uint32_t pSectorNum, char* pOutput) {
    SET_CS();

    // Send command 17 (READ_SINGLE_BLOCK)
    if (sdmmc_writeCommand(SDMMC_READ_SINGLE_BLOCK, SECTOR_TO_BYTE(pSectorNum), SDMMC_DEFAULT_CRC) != 0) {
        CLEAR_CS();
        return FALSE;
    }

    uint8_t response = 0;
    uint8_t retry = 0;

    // Wait for start-byte
    while(response != 0xFE) {
        response = spi_readByte();
        if (retry++ == 0xFF) {
            CLEAR_CS();
            return FALSE;
        }
    }

    // Read the block
    for (uint16_t i = 0; i < fBlockLength; i++) {
        pOutput[i] = spi_readByte();
    }

    // Ignore the CRC checksum
    spi_readByte();
    spi_readByte();

    CLEAR_CS();
    return TRUE;
}


uint8_t sdmmc_changeBlockLength(uint16_t pLength) {
    if (pLength == 0) {
        pLength = SDMMC_SECTOR_SIZE;
    }

    SET_CS();
    if (sdmmc_writeCommand(SDMMC_SET_BLOCKLEN, pLength, SDMMC_DEFAULT_CRC) != 0) {
        CLEAR_CS();
        return FALSE;
    }

    CLEAR_CS();
    fBlockLength = pLength;
    return TRUE;
}