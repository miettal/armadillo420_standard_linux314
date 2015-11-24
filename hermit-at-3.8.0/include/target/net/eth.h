#ifndef _HERMIT_TARGET_ETH_H_
#define _HERMIT_TARGET_ETH_H_

#include <net/net.h>

typedef enum _eth_proc_mode {
	ARP_WAIT,
	IP_WAIT,
} eth_proc_mode;

typedef enum _eth_protocol_id {
	ETH_PROTOCOL_IP = 0x0800,
	ETH_PROTOCOL_ARP = 0x0806,
	ETH_PROTOCOL_IPV6 = 0x86dd,
} eth_protocol_id;

#define ETH_FRAME_LEN 14
struct eth_frame {
	unsigned char dmac[6]; /* destination address */
	unsigned char smac[6]; /* source address */
	unsigned short protocol;
} __attribute__((packed));

extern const unsigned char broadcast_ipaddr[4];
extern const unsigned char broadcast_mac[6];

extern void enable_phy_module(struct net_device *dev);
extern void disable_phy_module(struct net_device *dev);
extern int eth_send(struct net_device *dev, const eth_protocol_id proto, const unsigned char *dst_mac, struct net_packet *pkt);
extern int eth_recv(struct net_device *dev, struct net_packet *pkt, int *timeout);
extern int eth_proc(struct net_device *dev, const int mode, struct net_packet *pkt);

#endif
