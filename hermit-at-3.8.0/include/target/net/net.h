#ifndef _HERMIT_TARGET_NET_NET_H_
#define _HERMIT_TARGET_NET_NET_H_

struct net_device;
struct net_packet;

#include <htypes.h>
#include <net/eth.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/udp.h>

struct net_packet {
	void *raw_data_addr;
	u32 raw_data_len;

	/* 1st frame */
	struct eth_frame *eth;

	/* 2nd frame */
	union {
		void *any;
		struct arp_frame *arp;
		struct ip_frame *ip;
	} f2;
	u32 f2len;

	/* 3rd frame */
	union {
		void *any;
		struct udp_frame *udp;
	} f3;
	u32 f3len;

	/* 4th frame */
	void *f4;
	u32 f4len;
};

struct net_device {
	unsigned int base_addr;

	unsigned char eth_ipaddr[4];
	unsigned char eth_mac[6];

	int (*init)(struct net_device *dev);
	int (*exit)(struct net_device *dev);

	/* operations */
	void (*set_mac)(struct net_device *dev, unsigned char *mac);
	void (*enable_phy)(struct net_device *dev);
	void (*disable_phy)(struct net_device *dev);
	int (*send)(struct net_device *dev, struct net_packet *pkt);
	int (*recv)(struct net_device *dev, struct net_packet *pkt, int *timeout);
};

extern void netdev_set_ipaddr(struct net_device *dev, unsigned char *ipaddr);
extern unsigned char *netdev_get_ipaddr(struct net_device *dev);
extern int netdev_init(struct net_device *dev);
extern int netdev_exit(struct net_device *dev);

#endif /* _HERMIT_TARGET_NET_NET_H_ */
