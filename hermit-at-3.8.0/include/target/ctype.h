
#ifndef _HERMIT_TARGET_CTYPE_H_
#define _HERMIT_TARGET_CTYPE_H_

#define CTYPE_DIGIT	(1<<0)
#define CTYPE_HEX	(1<<1)
#define CTYPE_LOWER	(1<<2)
#define CTYPE_UPPER	(1<<3)

int is_digit(char c);
int is_decimal(char c);
int is_hex(char c);
int is_lower(char c);
int is_upper(char c);
unsigned char to_lower(unsigned char c);

#endif /* _HERMIT_TARGET_CTYPE_H_ */
