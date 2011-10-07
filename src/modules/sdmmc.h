/**
 * \file sdmmc.h
 * \brief Library for handling SD/MMC memory cards
 * \author Martin Matysiak
 */

#ifndef SDMMC_H
    #define SDMMC_H

    #include "global.h"
    #include "protocols/spi.h"
    
    /// The default size of a sector on memory cards
    #define SDMMC_SECTOR_SIZE 512
    
    /// CMD0 - Change from SD into SPI mode
    #define SDMMC_GO_IDLE_STATE 0
    /// CMD1 - Initialize card
    #define SDMMC_SEND_OP_COND 1
    /// CMD16 - Set Blocklength
    #define SDMMC_SET_BLOCKLEN 16
    /// CMD17 - Read a single block
    #define SDMMC_READ_SINGLE_BLOCK 17
    /// CMD24 - Write a block
    #define SDMMC_WRITE_BLOCK 24
    
    /// Precalculated Checksum for CMD0
    #define SDMMC_GO_IDLE_STATE_CRC 0x95
    
    /// Default CRC value when in SPI mode (won't be checked)
    #define SDMMC_DEFAULT_CRC 0xFF
    
    /// Macro to convert from a sector count to a byte count
    #define SECTOR_TO_BYTE(pSector) pSector << 9 // << 9 equals * 512
    

    /**
     * \brief Initializes the SD/MMC-card. Locks the processor in case of error
     */
    void sdmmc_init();

    /**
     * \brief Writes the given data to the given sector
     *
     * Please note that writing operations will be done by default in 
     * 512 byte blocks unless the block length has been changed by using
     * sdmmc_changeBlockLength.
     *
     * \param pSectorNum an integer containing the index of the sector to which
     * the data should be written
     * \param pInput a string of characters which should be written
     * \return TRUE on success, otherwise FALSE
     */
    uint8_t sdmmc_writeSector(uint32_t pSectorNum, char* pInput);
    
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
     * Please note that reading operations will be done by default in 
     * 512 byte blocks unless the block langth has been changed by using 
     * sdmmc_changeBlockLength.
     *
     * \param pSectorNum an integer containing the index of the sector which
     * should be read out
     * \param pOutput A buffer to which the data will be written (make sure it's
     * at least 512 bytes large)
     * \return TRUE on success, otherwise FALSE
     */
    uint8_t sdmmc_readSector(uint32_t pSectorNum, char* pOutput);

    /**
     * \brief Changes the length of a block in read and write operations
     *
     * This command utilizes CMD16 to alter the block size which will be
     * used in read and write commands. Please note that this feature might
     * not work with every SD card! If it fails, the default block size
     * of 512 Bytes will remain unchanged.
     *
     * \param pLength the new length of a block. Set to 0 if default size should
     * be set.
     * \return TRUE on success, otherwise FALSE
     */
    uint8_t sdmmc_changeBlockLength(uint16_t pLength);
#endif


