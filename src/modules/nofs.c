/**
 * \file nofs.c
 * \brief Library for accessing a NoFS (NoFileSystem) on the memory card
 * \author Martin Matysiak
 */

#include "modules/nofs.h"

/// Index of the currently active sector
uint32_t fCurrentSector = 0;
/// Index which points to the current end of data inside the current sector
uint16_t fCurrentByte = 0;
/// Will be incremented upon every writeString call
uint8_t fWriteCount = 0;
/// Buffer which holds the currently active sector
char sectorBuf[NOFS_BUFFER_SIZE];

uint8_t nofs_init() { 
    do {
        // Search for the end of data (== ETX, 0x03) on the memory card, which
        // will be the position from where we will start writing
        sdmmc_readSector(fCurrentSector++, sectorBuf, NOFS_BUFFER_SIZE);

        for(fCurrentByte = 0; fCurrentByte < NOFS_BUFFER_SIZE; fCurrentByte++) {
            if (sectorBuf[fCurrentByte] == ETX) {
                break;
            }
        }

        // As long as the pointer points to the very last byte of the sector and
        // this byte isn't a ETX, we will get the next sector on the card
    } while((fCurrentByte == NOFS_BUFFER_SIZE) && (sectorBuf[fCurrentByte - 1] != ETX));

    // Undo the last "++"
    fCurrentSector--;

    return TRUE;
}

uint8_t nofs_writeString(char* pString) {
    // Copy data into the buffer. If the buffer is full, write it onto the
    // memory card and create a new empty one
    uint8_t i = 0;

    while (pString[i]) {
        if(fCurrentByte < NOFS_BUFFER_SIZE) {
            sectorBuf[fCurrentByte++] = pString[i++];
        } else {    
            // Sector full, write onto memory card and get the next one
            sdmmc_writeSector(fCurrentSector++, sectorBuf, NOFS_BUFFER_SIZE);
            _delay_ms(10);

            sdmmc_readSector(fCurrentSector, sectorBuf, NOFS_BUFFER_SIZE);

            // For safety's sake, write a ETX into the next sector
            sectorBuf[0] = ETX;
            sdmmc_writeSector(fCurrentSector, sectorBuf, NOFS_BUFFER_SIZE);

            fCurrentByte = 0;
        }
    }

    // Set the new end of data
    sectorBuf[fCurrentByte] = ETX;

    // Write buffer if this is the N-th call of the method
    if (fWriteCount++ == NOFS_WRITE_BUFFER_AFTER_NTH_COMMAND) {
        sdmmc_writeSector(fCurrentSector, sectorBuf, NOFS_BUFFER_SIZE);
        fWriteCount = 0;
    }

    return TRUE;
}
