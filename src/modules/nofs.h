/**
 * \file nofs.h
 * \brief Library for accessing a NoFS (NoFileSystem) on the memory card
 *
 * Some notes regarding the NoFS:
 * - The very first bytes of a device are always the same and equal to the
 *   NOFS_HEADER string (currently "k621.de"). This header is used to identify
 *   if a device contains a valid NoFS or not
 * - Starting with firmware version 1.5, the NOFS_HEADER is followed by an
 *   uint32_t value (MSB first), containing the number of a sector which is
 *   definitely _before_ the NOFS_TERMINAL byte, but as close to it as
 *   possible. This enhances the inital scan to determine the writing position
 *   as the application can jump directly to the specified sector and begin
 *   the scanning from there on.
 * - After this integer, the actual data starts. Basically the data bytes can 
 *   contain every value except for NOFS_TERMINAL. Preferrably the data should
 *   contain only printable ASCII characters (including \\r, \\n & \\t), though.
 * - The data must be terimanted with a NOFS_TERMINAL. This byte is placed 
 *   always in the _first_ byte of the sector which follows the last sector
 *   with actual data. Example: if the data goes until sector #42, byte #234,
 *   the NOFS_TERMINAL has to be written to sector #43, byte #0. The application
 *   should ensure that there is always at least one NOFS_TERMINAL present on
 *   the device (i.e. write the new NOFS_TERMINAL _before_ overwriting the 
 *   old one).
 *
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
     * \brief Initializes the NoFS. Locks the processor in case of error
     */
    void nofs_init();

    /**
     * \brief Appends a character string to the present data
     * 
     * \param pString A null-terminated character string which should be written
     * \return TRUE on success, FALSE otherwise
     */
    uint8_t nofs_writeString(char* pString);
    
    /**
     * \brief Writes the current data buffer onto the memory card
     * 
     * The buffer gets written to the sector specified by the global field
     * fCurrentSector. Additionally, there will be written a NOFS_TERMINAL
     * into the respective next sector (i.e. fCurrentSector + 1). This ensures
     * that the scanning algorithm during initialization won't fail to
     * find a terminal symbol.
     */
    void nofs_flush();
#endif


