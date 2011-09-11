/**
 * \file sdmmc.h
 * \brief Library for handling SD/MMC memory cards
 * \author Martin Matysiak
 */

#ifndef SDMMC_H
    #define SDMMC_H

    #include "global.h"
    #include "protocols/spi.h"

    /**
     * \brief Initializes the SD/MMC-card
     *
     * \return TRUE if initialization succeeded, otherwise FALSE
     */
    uint8_t sdmmc_init();

    /**
     * \brief Writes the given data to the given sector
     *
     * Please note that writing operations can only be done in 512 byte blocks
     * as one can not change the block size on a memory card.
     *
     * \param pSectorNum an integer containing the index of the sector to which
     * the data should be written
     * \param pInput a string of characters which should be written
     * \param pByteCount the length of the given character string (should equal
     * 512, will probably removed in the next firmware version)
     * \return TRUE on success, otherwise FALSE
     */
    uint8_t sdmmc_writeSector(uint32_t pSectorNum, char* pInput, uint16_t pByteCount);
    
    /**
     * \brief Sends a command to the SD/MMC-card
     * 
     * See http://www.sandisk.com/Assets/File/OEM/Manuals/SD_SDIO_specsv1.pdf 
     * for an overview of possible commands in SD (Apendix A) and SPI (Appendix
     * B) mode.
     *
     * \param pCommand The command which should be sent
     * \param pArgument Additional data for the command
     * \param pCrc The CRC-checksum for the given command (will be ignored if in SPI mode)
     * \return TRUE on success, otherwise FALSE
     */
    uint8_t sdmmc_writeCommand(uint8_t pCommand, uint32_t pArgument, uint8_t pCrc);

    /**
     * \brief Reads a sector from the SD/MMC-card
     *
     * Please note that reading operations can only be done in 512 byte blocks
     * as one can not change the block size on a memory card.
     *
     * \param pSectorNum an integer containing the index of the sector which
     * should be read out
     * \param pOutput A buffer to which the data will be written (make sure it's
     * at least 512 bytes large)
     * \param pByteCount The length of the pOutput buffer (must be at least 512,
     * parameter will probably be removed in the next firmware version)
     * \return TRUE on success, otherwise FALSE
     */
    uint8_t sdmmc_readSector(uint32_t pSectorNum, char* pOutput, uint16_t pByteCount);
#endif


