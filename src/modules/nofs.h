/**
 * \file nofs.h
 * \brief Library for accessing a NoFS (NoFileSystem) on the memory card
 * \author Martin Matysiak
 */

#ifndef NOFS_H
    #define NOFS_H

    #include "global.h"
    #include "modules/sdmmc.h"
    
    /// Size of the NoFS-Buffer (equals the block size on a memory card)
    #define NOFS_BUFFER_SIZE SDMMC_SECTOR_SIZE
    /// The buffer will be written on the memory card only every N writeString calls
    #define NOFS_WRITE_BUFFER_AFTER_NTH_COMMAND 10  
    /// The header which identifies a valid NoFS memory card
    #define NOFS_HEADER "k621.de" 
    /// The byte which is written to indicate the end of a NoFS partition
    #define NOFS_TERMINAL ETX 

    /**
     * \brief Initializes the NoFS
     *
     * Please note that the memory card has to be initialized already seperately
     * before (by calling sdmmc_init() or sth equivalent).
     *
     * \return TRUE on success, FALSE otherwise
     */
    uint8_t nofs_init();

    /**
     * \brief Appends a character string to the present data
     * 
     * \param pString A null-terminated character string which should be written
     * \return TRUE on success, FALSE otherwise
     */
    uint8_t nofs_writeString(char* pString);
#endif


