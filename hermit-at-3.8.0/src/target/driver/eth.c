#define CORE_NAME "eth"

#include <autoconf.h>
#include <hermit.h>
#include <io.h>
#include <string.h>

#include <net/net.h>
#include <net/eth.h>
#include <net/eth_util.h>
#include <net/arp.h>
#include <net/ip.h>
#include <arch/byteorder.h>

const unsigned char broadcast_ipaddr[4] = {0xff, 0xff, 0xff, 0xff};
const unsigned char broadcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#define LEN_RxBuf (1518+2+16)
#define LEN_TxBuf (1518+2+16)

void enable_phy_module(struct net_device *dev)
{
	dev->enable_phy(dev);
}

void disable_phy_module(struct net_device *dev)
{
	dev->disable_phy(dev);
}

int eth_send(struct net_device *dev, const eth_protocol_id proto, const unsigned char *dst_mac, struct net_packet *pkt)
{
	struct eth_frame ethfr;

	memset(&ethfr, 0, sizeof(struct eth_frame));
	memcpy(ethfr.dmac, dst_mac, 6);
	memcpy(ethfr.smac, dev->eth_mac, 6);
	ethfr.protocol = htons(proto);

	pkt->eth = &ethfr;

	return dev->send(dev, pkt);
}

int eth_recv(struct net_device *dev, struct net_packet *pkt, int *timeout)
{
	return dev->recv(dev, pkt, timeout);
}

int eth_proc(struct net_device *dev, const int mode, struct net_packet *pkt)
{
	int ret;

	static unsigned char buf[LEN_RxBuf];
	int timeout = 1000; /* 1s */

	if (mode == ARP_WAIT)
		timeout = 100; /* 100ms */

	while (1) {
		memzero(pkt, sizeof(struct net_packet));
		pkt->raw_data_addr = buf;
		ret = eth_recv(dev, pkt, &timeout);
		if (ret)
			return ret;

		pkt->eth = pkt->raw_data_addr;
		pkt->f2.any = pkt->raw_data_addr + ETH_FRAME_LEN;
		pkt->f2len = pkt->raw_data_len - ETH_FRAME_LEN;

		switch (ntohs(pkt->eth->protocol)) {
		case ETH_PROTOCOL_ARP:
			dev_vdbg("ARP\n");
			ret = arp_proc(dev, pkt);
			if (mode == ARP_WAIT) return 0;
			break;
		case ETH_PROTOCOL_IP:
			dev_vdbg("IP\n");
			if (mode == IP_WAIT) return 0;
			break;
		default:
			dev_vdbg("Unknown(%d)\n", ntohs(pkt->eth->protocol));
			break;
		}
	}
	return 0;
}
