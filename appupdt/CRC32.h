#pragma once

class CRC
{
private:
	/* Table of CRCs of all 8-bit messages. */
	unsigned long crc_table[256];

public:
	CRC() {
		/* Make the table for a fast CRC. */
		for (unsigned long n = 0; n < 256; n++) {
			unsigned long crc = (unsigned long)n;
			for (unsigned long k = 0; k < 8; k++) {
				if (crc & 1)
					crc = 0xedb88320L ^ (crc >> 1);
				else
					crc = crc >> 1;
			}
			crc_table[n] = crc;
		}
	}

	/* Update a running CRC with the bytes buf[0..len-1]--the CRC
	should be initialized to all 1's, and the transmitted value
	is the 1's complement of the final running CRC (see the
	crc() routine below)). */

	unsigned long update_crc(unsigned long crc, unsigned char *buf,
		unsigned long len) {
		while (len > 0) {
			crc = crc_table[(crc ^ (*buf)) & 0xff] ^ (crc >> 8);
			buf++; len--;
		}
		return crc;
	}

	/* Return the CRC of the bytes buf[0..len-1]. */
	unsigned long crc(unsigned char *buf, int len)
	{
		return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
	}
} crc;
