#define CORE_NAME "tftp"

#include <autoconf.h>
#if defined(CONFIG_ETHERNET_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <herrno.h>
#include <io.h>
#include <net/net.h>
#include <net/eth.h>
#include <net/udp.h>
#include <net/tftp.h>
#include <string.h>
#include <arch/byteorder.h>

#define TFTP_PROC_RETRY	5

static int create_read_packet(const char *filename, const char *mode,
			      unsigned char *pbuf, unsigned int pbuflen,
			      const char *blksize)
{
	int ptr = 0;
	int filename_len = strlen(filename);
	int mode_len = strlen(mode);
	unsigned short opcode = htons(TFTP_OP_READ);

	if (pbuflen < (2 + filename_len + 1 + mode_len + 1))
		return 0;

	memcpy(&pbuf[ptr], &opcode, 2);
	ptr += 2;

	memcpy(&pbuf[ptr], filename, filename_len);
	ptr += filename_len;

	pbuf[ptr++] = '\0';

	memcpy(&pbuf[ptr], mode, mode_len);
	ptr += mode_len;

	pbuf[ptr++] = '\0';

	if (blksize && blksize[0]) {
		int blksize_len = strlen(blksize);
		memcpy(&pbuf[ptr], "blksize", 7);
		ptr += 7;

		pbuf[ptr++] = '\0';

		memcpy(&pbuf[ptr], blksize, blksize_len);
		ptr += blksize_len;

		pbuf[ptr++] = '\0';
	}

	return ptr;
}

static inline int create_ack_packet(const unsigned short __block,
				    unsigned char *pbuf, unsigned int pbuflen)
{
	unsigned short opcode = htons(TFTP_OP_ACK);
	unsigned short block = htons(__block);

	if (pbuflen < 4)
		return 0;

	memcpy(&pbuf[0], &opcode, 2);
	memcpy(&pbuf[2], &block, 2);

	return 4;
}

static int tftp_send_ack(struct net_device *dev,
			 const unsigned char *server_ipaddr,
			 const unsigned short dst_port,
			 const unsigned short block)
{
	struct net_packet pkt;
	unsigned char data[4];
	u32 data_len;

	data_len = create_ack_packet(block, data, 4);
	memzero(&pkt, sizeof(struct net_packet));
	pkt.f4 = data;
	pkt.f4len = data_len;
	return udp_send(dev, server_ipaddr, dst_port, 30000, &pkt);
}

int tftp_get(struct net_device *dev, const unsigned char *server_ipaddr,
	     const char *filename,
	     size_t *__filesize,
	     addr_t __buf, unsigned int buflen, const char *blksize)
{
	int ret;
	int flag = 0;
	struct net_packet pkt, rx_pkt;
	unsigned char send_pbuf[1024];
	int send_pbuflen;
	unsigned char *recv_pbuf;
	unsigned int recv_pbuflen;
	unsigned short port;

	unsigned short opecode;
	unsigned short block;
	unsigned char *rcv_data;
	unsigned int rcv_data_len;
	unsigned char *filebuf = (unsigned char *)__buf;
	unsigned int filesize = 0;
	int progress = 0;
	int req_block = 1;
	int octets;
	int wait_oack = 0;
	int retry;

	if (blksize && blksize[0]) {
		octets = strtol(blksize, NULL, 0);
		wait_oack = 1;
	} else {
		octets = FIXED_BLOCK_SIZE;
	}

	dev_info_r("Filename : %s\n", filename);
	send_pbuflen = create_read_packet(filename, MODE_BIN, send_pbuf,
					  1024, blksize);
	if (send_pbuflen == 0)
		return -H_EINVAL;

	memzero(&pkt, sizeof(struct net_packet));
	pkt.f4 = &send_pbuf;
	pkt.f4len = send_pbuflen;
	ret = udp_send(dev, server_ipaddr, TFTP_PORT_NO, 30000, &pkt);
	if (ret)
		return ret;

	if (wait_oack) {
		ret = udp_proc(dev, server_ipaddr, 30000, &port, &rx_pkt);
		if (ret)
			return ret;

		recv_pbuf = rx_pkt.f4;
		recv_pbuflen = rx_pkt.f4len;

		opecode = ntohs(*(unsigned short *)(recv_pbuf + 0));
		switch (opecode) {
		case TFTP_OP_OPTION:
			tftp_send_ack(dev, server_ipaddr, port, 0);
			break;

		case TFTP_OP_ERROR:
		{
			unsigned short errcode;
			unsigned char *errmsg;
			errcode = ntohs(*(unsigned short *)(recv_pbuf + 2));
			errmsg = recv_pbuf + 4;
			dev_err("[%d]: %s\n", errcode, errmsg);
			return -H_EIO;
		}
		default:
			dev_err("Could not receive OACK.\n");
			return -H_EIO;
		}
	}

	retry = TFTP_PROC_RETRY;
	do {
		if ((progress++ % 10) == 0)
			dev_info_r("."); /* progress */

		ret = udp_proc(dev, server_ipaddr, 30000, &port, &rx_pkt);
		if (ret) {
			if (ret == -H_EAGAIN) {
				if (--retry <= 0)
					return -H_ETIMEDOUT;
				tftp_send_ack(dev, server_ipaddr, port, req_block - 1);
				continue;
			}
			return ret;
		}

		recv_pbuf = rx_pkt.f4;
		recv_pbuflen = rx_pkt.f4len;

		opecode = ntohs(*(unsigned short *)(recv_pbuf + 0));
		switch (opecode) {
		case TFTP_OP_DATA:
			block = ntohs(*(unsigned short *)(recv_pbuf + 2));

			if (req_block != block) {
				dev_dbg("receive unknown block: %d (request: %d)\n",
					req_block, block);
				tftp_send_ack(dev, server_ipaddr, port, req_block - 1);
				continue;
			} else {
				req_block++;
			}

			rcv_data_len = recv_pbuflen - 4;
			rcv_data = recv_pbuf + 4;

			tftp_send_ack(dev, server_ipaddr, port, block);

			if (rcv_data_len < octets)
				flag = 1;

			if (rcv_data_len == 0)
				break;

			if (buflen - filesize < rcv_data_len) {
				dev_err("Out of memory\n");
				return -H_EOVERFLOW;
			}

			memcpy(&filebuf[filesize], rcv_data, rcv_data_len);

			filesize += rcv_data_len;
			break;

		case TFTP_OP_ERROR:
		{
			unsigned short errcode;
			unsigned char *errmsg;
			errcode = ntohs(*(unsigned short *)(recv_pbuf + 2));
			errmsg = recv_pbuf + 4;
			dev_err("[%d]: %s\n", errcode, errmsg);
			return -H_EIO;
		}

		default:
			break;
		}
		retry = TFTP_PROC_RETRY;
	} while (flag == 0);

	dev_info_r("\nFilesize : %d\n\n", filesize);

	*__filesize = filesize;

	return 0;
}
