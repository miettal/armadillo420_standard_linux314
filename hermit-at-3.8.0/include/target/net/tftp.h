#ifndef _HERMIT_TFTP_H_
#define _HERMIT_TFTP_H_

#include <htypes.h>

#define TFTP_PORT_NO 69

#define FIXED_BLOCK_SIZE 512

#define MODE_ASCII "netascii"
#define MODE_BIN   "octet"

typedef enum _tftp_opcode_id {
	TFTP_OP_READ  = 1,
	TFTP_OP_WRITE = 2,
	TFTP_OP_DATA  = 3,
	TFTP_OP_ACK   = 4,
	TFTP_OP_ERROR = 5,
	TFTP_OP_OPTION= 6,
} tftp_opecode;

extern int tftp_get(struct net_device *dev, const unsigned char *server_ipaddr, const char *filename, size_t *__filesize, addr_t __buf, unsigned int buflen, const char *blksize);

#endif
