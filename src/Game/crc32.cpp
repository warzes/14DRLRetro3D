#include "stdafx.h"
#include "crc32.h"

#if (STANDALONE == 0)
static long crctab32[256] = { 0 };  //SEE CRC32.C
#endif

#if (STANDALONE == 0)
#define updatecrc32(c,crc) crc=(crctab32[((c)^crc)&255]^(((unsigned)crc)>>8))
#define updateadl32(c,crc) \
{  c += (crc&0xffff); if (c   >= 65521) c   -= 65521; \
	crc = (crc>>16)+c; if (crc >= 65521) crc -= 65521; \
	crc = (crc<<16)+c; \
}
#endif

void initcrc32()
{
	long i, j, k;
	for( i = 255; i >= 0; i-- )
	{
		k = i;
		for( j = 8; j; j-- ) k = ((unsigned long)k >> 1) ^ ((-(k & 1)) & 0xedb88320);
		crctab32[i] = k;
	}
}

long getcrc32z(long crc32, unsigned char *buf) { long i; for( i = 0; buf[i]; i++ ) updatecrc32(buf[i], crc32); return(crc32); }