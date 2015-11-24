/**
 * fls - find last bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
unsigned long fls(unsigned long word)
{
	int num = 0;

#if BITS_PER_LONG == 64
	if (word & 0xffffffff00000000) {
		num += 32;
		word >>= 32;
	}
#endif
	if (word & 0xffff0000) {
		num += 16;
		word >>= 16;
	}
	if (word & 0xff00) {
		num += 8;
		word >>= 8;
	}
	if (word & 0xf0) {
		num += 4;
		word >>= 4;
	}
	if (word & 0xc) {
		num += 2;
		word >>= 2;
	}
	if (word & 0x2)
		num += 1;

	return num;
}
