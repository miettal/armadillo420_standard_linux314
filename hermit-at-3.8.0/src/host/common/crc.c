/*
 * Implementation of CRC-32 from the online Compression FAQ.
 */

#include "crc.h"

/*
 * Polynomial used to build table for parallel byte-at-a-time CRC-32.
 */
#define CRC32_POLY 0x04c11db7

static uint32_t crc32tab [256];

void init_crc32(void)
{
	int i, j;
	uint32_t c;

	for (i = 0; i < sizeof crc32tab / sizeof *crc32tab; ++i) {
		for (c = i << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
		crc32tab[i] = c;
	}
}

uint32_t crc32(const unsigned char *buf, unsigned size)
{
	const unsigned char *p;
	uint32_t crc;

	crc = 0xffffffff;	/* preload shift register, per CRC-32 spec */
	for (p = buf; size > 0; ++p, --size)
		crc = (crc << 8) ^ crc32tab[(crc >> 24) ^ *p];
	return ~crc;	/* transmit complement, per CRC-32 spec */
}

