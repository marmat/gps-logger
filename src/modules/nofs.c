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

void nofs_init() { 
    /*
        Steps of initialization:
        1) Call initialization of underlying SDMMC interface
        2) Read the first sector
        3) Check if sector starts with NOFS_HEADER (display error if not)
        4) Get position of last scan for writing position
        5) Jump to this position and scan for NOFS_TERMINAL from there on
        6) Jump back to first sector and update last writing position
        7) Jump forward to writing position, initialization finished.
    */
    
    // Step 1
    sdmmc_init();
    
    // Step 2
    sdmmc_readSector(0, sectorBuf);
    
    // Step 3
    if (!strStartsWith(sectorBuf, NOFS_HEADER)) {
        error(ERROR_NOFS);
    }
    
    // Step 4
    // TODO //
    
    // Step 5
    // TODO //
    
    // Step 6
    // TODO //
    
    // Step 7
    // TODO //
    
    // Old code:
    do {
        // Search for the end of data (== ETX, 0x03) on the memory card, which
        // will be the position from where we will start writing
        sdmmc_readSector(fCurrentSector++, sectorBuf);

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
            sdmmc_writeSector(fCurrentSector++, sectorBuf);
            _delay_ms(10);

            sdmmc_readSector(fCurrentSector, sectorBuf);

            // For safety's sake, write a ETX into the next sector
            sectorBuf[0] = ETX;
            sdmmc_writeSector(fCurrentSector, sectorBuf);

            fCurrentByte = 0;
        }
    }

    // Set the new end of data
    sectorBuf[fCurrentByte] = ETX;

    // Write buffer if this is the N-th call of the method
    if (fWriteCount++ == NOFS_WRITE_BUFFER_AFTER_NTH_COMMAND) {
        sdmmc_writeSector(fCurrentSector, sectorBuf);
        fWriteCount = 0;
    }

    return TRUE;
}

void nofs_flush() {
    // Remember the first byte as we will replace it with the NOFS_TERMINAL
    // temporarily to write the current+1 sector
    char temp = sectorBuf[0];
    sectorBuf[0] = NOFS_TERMINAL;
    sdmmc_writeSector(fCurrentSector + 1, sectorBuf);
    
    _delay_ms(10);
    
    // Now write the actual current sector
    sectorBuf[0] = temp;
    sdmmc_writeSector(fCurrentSector, sectorBuf);
}
