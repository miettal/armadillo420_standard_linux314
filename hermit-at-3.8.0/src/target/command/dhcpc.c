#define CORE_NAME "dhcpc"

#include <autoconf.h>
#include <hermit.h>
#include <string.h>
#include <net/net.h>
#include <net/eth.h>
#include <net/udp.h>
#include <net/dhcp.h>
#include <arch/byteorder.h>

#define MAGIC_COOKIE		0x63825363

/* bootp Operation Code */
#define OP_BOOTREQUEST		0x01
#define OP_BOOTREPLY		0x02

/* hardware type */
#define HTYPE_ETHER		0x01

/* RFC 1497 Vendor Extensions */
#define DHCP_PADDING		0x00
#define DHCP_REQUESTED_IP	0x32
#define DHCP_MESSAGE_TYPE	0x35
#define DHCP_SERVER_ID		0x36
#define DHCP_CLIENT_ID		0x3d
#define DHCP_OPTION_END		0xff

/* DHCP Message Type */
#define DHPCDISCOVER		0x01
#define DHCPOFFER		0x02
#define DHCPREQUEST		0x03
#define DHCPDECLINE		0x04
#define DHCPACK			0x05
#define DHCPNAK			0x06
#define DHCPRELEASE		0x07
#define DHCPINFORM		0x08

struct bootp_opt {
	unsigned char code;
	unsigned char len;
	unsigned char data;
};

static struct bootp_opt *find_bootp_option(struct bootp_pkt *bootp,
					   unsigned char option)
{
	unsigned char *ptr = bootp->option;

	for (; ptr < bootp->option + BOOTP_OPTION_LEN;) {
		if (*ptr == option)
			return (struct bootp_opt *)ptr;
		if (*ptr == DHCP_OPTION_END)
			return NULL;
		if (*ptr == DHCP_PADDING) {
			ptr++;
			continue;
		}
		ptr += ptr[1] + 1 + 1;
	}
	return NULL;
}

static int dhcp_discover(struct net_device *dev, struct bootp_pkt *offer)
{
	struct net_packet pkt, rx_pkt;
	struct bootp_pkt bootp;
	struct bootp_opt *opt;
	int offset = 0;
	int ret;
	unsigned short port;

	memzero(&bootp, sizeof(struct bootp_pkt));

	bootp.op = OP_BOOTREQUEST;
	bootp.htype = HTYPE_ETHER;
	bootp.hlen = 6;
	memcpy(bootp.hw_addr, dev->eth_mac, 6);

	/* Magic Cookie */
	bootp.magic = htonl(MAGIC_COOKIE);
	/* DHCP Message Type: DHCP Discover */
	bootp.option[offset++] = DHCP_MESSAGE_TYPE;
	bootp.option[offset++] = 0x1;
	bootp.option[offset++] = DHPCDISCOVER;
	/* Client identifier */
	bootp.option[offset++] = DHCP_CLIENT_ID;
	bootp.option[offset++] = 0x7;
	bootp.option[offset++] = HTYPE_ETHER;
	memcpy(&bootp.option[offset], dev->eth_mac, 6); offset += 6;
	/* End Option */
	bootp.option[offset++] = DHCP_OPTION_END;
	bootp.xid = 0x12345678;

	memzero(&pkt, sizeof(struct net_packet));
	pkt.f4 = &bootp;
	pkt.f4len = sizeof(struct bootp_pkt);
	ret = udp_send(dev, broadcast_ipaddr,
		       BOOTP_SERVER_PORT, BOOTP_CLIENT_PORT,
		       &pkt);
	if (ret)
		return -1;

	memzero(&rx_pkt, sizeof(struct net_packet));
	ret = udp_proc(dev, broadcast_ipaddr, BOOTP_CLIENT_PORT, &port,
		       &rx_pkt);
	if (ret)
		return -1;

	memcpy(offer, rx_pkt.f4, sizeof(struct bootp_pkt));

	if (ntohl(offer->magic) != MAGIC_COOKIE) {
		dev_err("invalid magic cookie.\n");
		return -1;
	}

	opt = find_bootp_option(offer, DHCP_MESSAGE_TYPE);
	if (opt == NULL)
		return -1;
	if (opt->data != DHCPOFFER)
		return -1;

	dev_dbg("offerd address: %d.%d.%d.%d\n",
		offer->your_ip[0], offer->your_ip[1],
		offer->your_ip[2], offer->your_ip[3]);

	return 0;
}

static int dhcp_request(struct net_device *dev, struct bootp_pkt *offer)
{
	struct net_packet pkt, rx_pkt;
	struct bootp_pkt bootp, *reply;
	struct bootp_opt *opt;
	int offset = 0;
	int ret;
	unsigned short port;

	trace();

	memzero(&bootp, sizeof(struct bootp_pkt));

	bootp.op = OP_BOOTREQUEST;
	bootp.htype = HTYPE_ETHER;
	bootp.hlen = 6;
	memcpy(bootp.hw_addr, dev->eth_mac, 6);
	memcpy(bootp.your_ip, offer->your_ip, 4);

	/* Magic Cookie */
	bootp.magic = htonl(MAGIC_COOKIE);
	/* DHCP Message Type: DHCP Request */
	bootp.option[offset++] = DHCP_MESSAGE_TYPE;
	bootp.option[offset++] = 0x1;
	bootp.option[offset++] = DHCPREQUEST;
	/* Client identifier */
	bootp.option[offset++] = DHCP_CLIENT_ID;
	bootp.option[offset++] = 0x7;
	bootp.option[offset++] = HTYPE_ETHER;
	memcpy(&bootp.option[offset], dev->eth_mac, 6); offset += 6;
	/* Request IP */
	bootp.option[offset++] = DHCP_REQUESTED_IP;
	bootp.option[offset++] = 0x4;
	memcpy(&bootp.option[offset], offer->your_ip, 4); offset += 4;
	/* Server ID */
	opt = find_bootp_option(offer, DHCP_SERVER_ID);
	if (opt) {
		bootp.option[offset++] = DHCP_SERVER_ID;
		bootp.option[offset++] = 0x4;
		memcpy(&bootp.option[offset], &opt->data, 4); offset += 4;
	}
	/* End Option */
	bootp.option[offset++] = DHCP_OPTION_END;
	bootp.xid = 0x12345678;

	memzero(&pkt, sizeof(struct net_packet));
	pkt.f4 = &bootp;
	pkt.f4len = sizeof(struct bootp_pkt);
	ret = udp_send(dev, broadcast_ipaddr,
		       BOOTP_SERVER_PORT, BOOTP_CLIENT_PORT,
		       &pkt);
	if (ret)
		return -1;

	memzero(&rx_pkt, sizeof(struct net_packet));
	ret = udp_proc(dev, broadcast_ipaddr, BOOTP_CLIENT_PORT, &port,
		       &rx_pkt);
	if (ret)
		return -1;

	reply = rx_pkt.f4;

	if (ntohl(reply->magic) != MAGIC_COOKIE) {
		dev_err("invalid magic cookie.\n");
		return -1;
	}

	opt = find_bootp_option(reply, DHCP_MESSAGE_TYPE);
	if (opt == NULL)
		return -1;
	if (opt->data != DHCPACK)
		return -1;

	dev_dbg("lease address: %d.%d.%d.%d\n",
		reply->your_ip[0], reply->your_ip[1],
		reply->your_ip[2], reply->your_ip[3]);

	return 0;
}

int dhcp_get_ipaddr(struct net_device *dev, unsigned char *ipaddr)
{
	struct bootp_pkt offer;
	int ret;

	trace();

	memzero(&offer, sizeof(struct bootp_pkt));
	ret = dhcp_discover(dev, &offer);
	if (ret)
		return ret;

	memcpy(ipaddr, offer.your_ip, 4);
	return dhcp_request(dev, &offer);
}
