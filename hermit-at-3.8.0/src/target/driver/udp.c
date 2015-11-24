#define CORE_NAME "udp"

#include <autoconf.h>
#if defined(CONFIG_ETHERNET_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <io.h>

#include <net/net.h>
#include <net/eth.h>
#include <net/eth_util.h>
#include <net/ip.h>
#include <net/udp.h>
#include <string.h>
#include <arch/byteorder.h>

static unsigned short udp_checksum(const struct ip_pseudo_header *__pheader,
				   const struct net_packet *pkt)
{
	unsigned short *buf;
	unsigned long sum;
	int i;

	/* pseudo */
	buf = (unsigned short *)__pheader;
	for (sum = 0, i = 0; i < 6; i++)
		sum += buf[i];

	/* udp */
	buf = (unsigned short *)pkt->f3.udp;
	for (i = 0; i < (sizeof(struct udp_frame) / 2); i++)
		sum += buf[i];

	/* f4 */
	buf = (unsigned short *)pkt->f4;
	for (i = 0; i < (pkt->f4len / 2); i++)
		sum += buf[i];
	if (pkt->f4len % 2)
		sum += (buf[i] & 0x00ff);

	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);

	return (unsigned short)~sum;
}

int udp_send(struct net_device *dev, const unsigned char *dst_ipaddr,
	     const unsigned short dst_port,
	     const unsigned short src_port,
	     struct net_packet *pkt)
{
	struct udp_frame udpfr;
	unsigned int udpfr_len = sizeof(struct udp_frame);
	struct ip_pseudo_header pheader;
	unsigned short checksum;

	/* create pseudo header */
	memcpy(pheader.dst_ipaddr, dst_ipaddr, 4);
	memcpy(pheader.src_ipaddr, netdev_get_ipaddr(dev), 4);
	pheader.zero = 0;
	pheader.protocol = IP_PROTOCOL_UDP;
	pheader.data_len = htons((unsigned short)(pkt->f4len + udpfr_len));

	/* create udp header */
	udpfr.src_port = htons(src_port);
	udpfr.dst_port = htons(dst_port);
	udpfr.data_len = htons((unsigned short)(pkt->f4len + udpfr_len));
	udpfr.checksum = 0;

	pkt->f3.udp = &udpfr;
	pkt->f3len = udpfr_len;

	checksum = udp_checksum(&pheader, pkt);
	udpfr.checksum = checksum;

	return ip_send(dev, dst_ipaddr, IP_PROTOCOL_UDP, pkt);
}

int udp_proc(struct net_device *dev, const unsigned char *src_ipaddr,
	     const unsigned short dst_port, unsigned short *src_port,
	     struct net_packet *pkt)
{
	struct udp_frame *udpfr;
	struct ip_frame *ipfr;
	struct ip_pseudo_header pheader;
	unsigned short checksum;
	unsigned short tmp;
	int ret;

	while (1) {
		ret = ip_proc(dev, UDP_WAIT, pkt);
		if (ret)
			return ret;

		if (pkt->f3len < sizeof(struct udp_frame))
			continue;

		ipfr = pkt->f2.ip;
		udpfr = pkt->f3.udp;
		pkt->f4 = pkt->f3.any + sizeof(struct udp_frame);
		pkt->f4len = pkt->f3len - sizeof(struct udp_frame);

		if (src_port != 0)
			*src_port = ntohs(udpfr->src_port);

		/* create pseudo header */
		memcpy(pheader.dst_ipaddr, ipfr->dst_ipaddr, 4);
		memcpy(pheader.src_ipaddr, ipfr->src_ipaddr, 4);
		pheader.zero = 0;
		pheader.protocol = IP_PROTOCOL_UDP;
		pheader.data_len = htons(pkt->f3len);

		checksum = udpfr->checksum;
		if (checksum != 0) {
			udpfr->checksum = 0;

			tmp = udp_checksum(&pheader, pkt);
			if (tmp == 0)
				tmp = 0xffff;

			if (checksum != tmp)
				continue;
		}

		dev_dbg("src_port: %d\n", ntohs(udpfr->src_port));
		dev_dbg("dst_port: %d\n", ntohs(udpfr->dst_port));

		if (dst_port == ntohs(udpfr->dst_port))
			return 0;
	}
	return 0;
}
