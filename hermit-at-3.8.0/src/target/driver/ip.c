#define CORE_NAME "ip"

#include <autoconf.h>
#if defined(CONFIG_ETHERNET_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <herrno.h>
#include <io.h>

#include <net/net.h>
#include <net/eth.h>
#include <net/eth_util.h>
#include <net/arp.h>
#include <net/ip.h>
#include <string.h>
#include <arch/byteorder.h>

static unsigned short ip_checksum(const void *__buf,
				  const unsigned int buflen)
{
	unsigned short *buf = (unsigned short *)__buf;
	unsigned long sum;
	int i;

	for (sum = 0, i = 0; i < (buflen / 2); i++)
		sum += buf[i];

	if(buflen % 2)
		sum += (buf[i] & 0x00ff);

	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);

	return (unsigned short)~sum;
}

int ip_send(struct net_device *dev, const unsigned char *dst_ipaddr,
	    const unsigned char protocol, struct net_packet *pkt)
{
	static unsigned short packet_id = 0;
	struct ip_frame ipfr;
	unsigned int ipfr_len = sizeof(struct ip_frame);
	unsigned char dst_mac[6];
	int ret;

	if (memcmp(dst_ipaddr, broadcast_ipaddr, 4) == 0) {
		memcpy(dst_mac, broadcast_mac, 6);
	} else {
		ret = arp_search(dev, dst_ipaddr, dst_mac);
		if (ret)
			return ret;
	}

	memset(&ipfr, 0, ipfr_len);
	ipfr.version = 4;
	ipfr.header_len = (LEN_IP_Header >> 2);
	ipfr.data_len = htons(ipfr_len + pkt->f4len + pkt->f3len);
	ipfr.id = htons(packet_id++);
	ipfr.flag = htons(IP_DF); /* Don't Flagment */
	ipfr.ttl = 64;
	ipfr.protocol = protocol;
	memcpy(ipfr.src_ipaddr, netdev_get_ipaddr(dev), 4);
	memcpy(ipfr.dst_ipaddr, dst_ipaddr, 4);

	ipfr.checksum = ip_checksum(&ipfr, ipfr_len);

	pkt->f2.ip = &ipfr;
	pkt->f2len = ipfr_len;

	return eth_send(dev, ETH_PROTOCOL_IP, dst_mac, pkt);
}

int ip_proc(struct net_device *dev, const int mode, struct net_packet *pkt)
{
	struct ip_frame *ipfr;
	unsigned short checksum;
	unsigned short tmp;
	int ret;

	while(1) {
		ret = eth_proc(dev, IP_WAIT, pkt);
		if (ret)
			return ret;

		if (pkt->f2len < sizeof(struct ip_frame))
			continue;

		ipfr = pkt->f2.ip;

		if ((ntohs(ipfr->flag) & ~IP_OFFSET) & IP_MF) {
			dev_dbg("not support IP fragment: %x\n",
				ntohs(ipfr->flag));
			continue;
		}

		checksum = ipfr->checksum;
		if (checksum != 0) {
			ipfr->checksum = 0;
			tmp = ip_checksum(ipfr, sizeof(struct ip_frame));
			if (tmp == 0)
				tmp = 0xffff;

			if (checksum != tmp)
				continue;
		}

		pkt->f3.any = pkt->f2.any + (ipfr->header_len << 2);
		pkt->f3len = ntohs(ipfr->data_len) - (ipfr->header_len << 2);

		switch (ipfr->protocol) {
		case IP_PROTOCOL_ICMP:
			dev_dbg("ICMP\n");
			if (mode == ICMP_WAIT) return 0;
			break;

		case IP_PROTOCOL_TCP:
			dev_dbg("TCP\n");
			if (mode == TCP_WAIT) return 0;
			break;

		case IP_PROTOCOL_UDP:
			dev_dbg("UDP\n");
			if (mode == UDP_WAIT) return 0;
			break;

		default:
			break;
		}
	}
	return 0;
}
