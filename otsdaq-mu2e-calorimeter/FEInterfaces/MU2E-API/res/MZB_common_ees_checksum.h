#ifndef _MZB_COMMON_CHECKSUM_H
#define _MZB_COMMON_CHECKSUM_H




/**
 *    @brief        Compute checksum on memory
 *    @param        src  pointer to buffer
 *    @param        len lenght of the buffer
 *    @return       checksum value
 */
unsigned ees_chksum(void *src, int len);


#endif