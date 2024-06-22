#ifndef _MZB_COMMON_CHECKSUM_C
#define _MZB_COMMON_CHECKSUM_C

/**
 *   @brief   Compute checksum
 *   @param   ptr  pointer to buffer data
 *   @param   len  length of the buffer (it can be any number)
 *   @return  Checksum value
 */
unsigned ees_chksum(void *ptr, int len) {
  unsigned *src = (unsigned*)ptr;
  unsigned sum;
  unsigned tmp;

  for(sum=0; len>=4; len-=4)
    sum += *src++;

  if (len) {
    tmp = 0;
    memcpy(&tmp, src, len);
    sum += tmp;
  }

  return sum;
}


#endif