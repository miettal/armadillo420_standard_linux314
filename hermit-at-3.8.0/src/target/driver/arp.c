#define CORE_NAME "arp"

#include <hermit.h>
#include <herrno.h>
#include <io.h>
#include <string.h>

#include <net/net.h>
#include <net/eth_util.h>
#include <net/eth.h>
#include <net/arp.h>
#include <arch/byteorder.h>

#define LEN_ARP_Cache 16
static struct arp_cache_table arp_cache[LEN_ARP_Cache];
static int arp_cache_idx = 0;

void arp_cache_clear(void)
{
	arp_cache_idx = 0;
	memset(&arp_cache, 0, sizeof(struct arp_cache_table) * LEN_ARP_Cache);
}

static int arp_cache_add(const unsigned char *target_ipaddr,
			 const unsigned char *target_mac)
{
	int i;

	for (i=0; i<LEN_ARP_Cache; i++)
		if (memcmp(arp_cache[i].ipaddr, target_ipaddr, 4) == 0)
			break;

	if (i == LEN_ARP_Cache)
		i = (arp_cache_idx++) & 0xf;

	memcpy(arp_cache[i].mac, target_mac, 6);
	memcpy(arp_cache[i].ipaddr, target_ipaddr, 4);

	return 0;
}

static int arp_cache_search(const unsigned char *target_ipaddr,
			    unsigned char *target_mac)
{
	int i;

	for (i=0; i<LEN_ARP_Cache; i++) {
		if (memcmp(arp_cache[i].ipaddr, target_ipaddr, 4) == 0) {
			memcpy(target_mac, arp_cache[i].mac, 6);
			return 0;
		}
	}
	return -1;
}

static int arp_request(struct net_device *dev, const unsigned char *target_ipaddr)
{
	struct net_packet pkt;
	struct arp_frame arpfr;

	memset(&arpfr, 0, sizeof(struct arp_frame));

	arpfr.htype = htons(ARP_HTYPE_ETH);
	arpfr.ptype = htons(ARP_PTYPE_IP);
	arpfr.hlen = ARP_HLEN_ETH;
	arpfr.plen = ARP_PLEN_IPV4;
	arpfr.opecode = htons(ARP_OP_REQ);
	memcpy(arpfr.host_mac, dev->eth_mac, 6);
	memcpy(arpfr.host_ipaddr, netdev_get_ipaddr(dev), 4);
	memset(arpfr.target_mac, 0, 6);
	memcpy(arpfr.target_ipaddr, target_ipaddr, 4);

	memzero(&pkt, sizeof(struct net_packet));
	pkt.f2.arp = &arpfr;
	pkt.f2len = sizeof(struct arp_frame);

	return eth_send(dev, ETH_PROTOCOL_ARP, broadcast_mac, &pkt);
}

int arp_search(struct net_device *dev, const unsigned char *target_ipaddr, unsigned char *target_mac)
{
	struct net_packet pkt;
	int i;
	int ret;

	ret = arp_cache_search(target_ipaddr, target_mac);
	if (ret == 0)
		return 0;

	for (i=0; i<5; i++) {
		ret = arp_request(dev, target_ipaddr);
		ret = eth_proc(dev, ARP_WAIT, &pkt);
		if (ret == -H_EAGAIN)
			continue;
		if (ret)
			return ret;
		ret = arp_cache_search(target_ipaddr, target_mac);
		if (ret == 0)
			return 0;
	}
	return -H_EIO;
}

int arp_proc(struct net_device *dev, struct net_packet *pkt)
{
	struct net_packet tx_pkt;
	struct arp_frame send_arpfr;

	if (pkt->f2len < sizeof(struct arp_frame))
		return -H_EINVAL;

	dev_dbg("htype     : %p\n", ntohs(pkt->f2.arp->htype));
	dev_dbg("ptype     : %p\n", ntohs(pkt->f2.arp->ptype));
	dev_dbg("hlen      : %p\n", pkt->f2.arp->hlen);
	dev_dbg("plen      : %p\n", pkt->f2.arp->plen);
	dev_dbg("opcode    : %p\n", ntohs(pkt->f2.arp->opecode));
	dev_dbg("hostmac   : %b:%b:%b:%b:%b:%b\n",
		pkt->f2.arp->host_mac[0], pkt->f2.arp->host_mac[1],
		pkt->f2.arp->host_mac[2], pkt->f2.arp->host_mac[3],
		pkt->f2.arp->host_mac[4], pkt->f2.arp->host_mac[5]);
	dev_dbg("hostip    : %d.%d.%d.%d\n",
		pkt->f2.arp->host_ipaddr[0], pkt->f2.arp->host_ipaddr[1],
		pkt->f2.arp->host_ipaddr[2], pkt->f2.arp->host_ipaddr[3]);
	dev_dbg("targetmac : %b:%b:%b:%b:%b:%b\n",
		pkt->f2.arp->target_mac[0], pkt->f2.arp->target_mac[1],
		pkt->f2.arp->target_mac[2], pkt->f2.arp->target_mac[3],
		pkt->f2.arp->target_mac[4], pkt->f2.arp->target_mac[5]);
	dev_dbg("targetip  : %d.%d.%d.%d\n",
		pkt->f2.arp->target_ipaddr[0], pkt->f2.arp->target_ipaddr[1],
		pkt->f2.arp->target_ipaddr[2], pkt->f2.arp->target_ipaddr[3]);

	switch (ntohs(pkt->f2.arp->opecode)) {
	case ARP_OP_REQ:
		if (memcmp(pkt->f2.arp->target_ipaddr, netdev_get_ipaddr(dev), 4) == 0) {
			memset(&send_arpfr, 0 , sizeof(struct arp_frame));
			send_arpfr.htype = htons(ARP_HTYPE_ETH);
			send_arpfr.ptype = htons(ARP_PTYPE_IP);
			send_arpfr.hlen = ARP_HLEN_ETH;
			send_arpfr.plen = ARP_PLEN_IPV4;
			send_arpfr.opecode = htons(ARP_OP_REPLY);
			memcpy(send_arpfr.host_mac, dev->eth_mac, 6);
			memcpy(send_arpfr.host_ipaddr, netdev_get_ipaddr(dev), 4);
			memcpy(send_arpfr.target_mac, pkt->f2.arp->host_mac, 6);
			memcpy(send_arpfr.target_ipaddr,
			       pkt->f2.arp->host_ipaddr, 4);

			memzero(&tx_pkt, sizeof(struct net_packet));
			tx_pkt.f2.arp = &send_arpfr;
			tx_pkt.f2len = sizeof(struct arp_frame);

			eth_send(dev, ETH_PROTOCOL_ARP, pkt->f2.arp->host_mac, &tx_pkt);
		}
		return 0;

	case ARP_OP_REPLY:
		if (memcmp(pkt->f2.arp->target_ipaddr, netdev_get_ipaddr(dev), 4) == 0)
			arp_cache_add(pkt->f2.arp->host_ipaddr,
				      pkt->f2.arp->host_mac);
		return 0;

	case ARP_OP_RARPREQ:
	case ARP_OP_RARPREPLY:
	default:
		return -H_ENOSYS;
	}
}
