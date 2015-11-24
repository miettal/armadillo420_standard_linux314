#ifndef _HERMIT_ARP_H_
#define _HERMIT_ARP_H_

#define ARP_CACHE_SURVIVAL_TIME 20 /* [minute] */

struct arp_cache_table {
	unsigned char mac[6];
	unsigned char ipaddr[4];
};

typedef enum _arp_htype_id {
	ARP_HTYPE_ETH    = 0x0001, /* Ethernet */
	ARP_HTYPE_TOKENR = 0x0004, /* TokenRing */
	ARP_HTYPE_ATM    = 0x0010, /* ATM */
} arp_htype_id;

typedef enum _arp_ptype_id {
	ARP_PTYPE_IP = 0x0800,
} arp_ptype_id;

#define ARP_HLEN_ETH  0x06
#define ARP_PLEN_IPV4 0x04

typedef enum _arp_opecode {
	ARP_OP_REQ       = 0x0001,
	ARP_OP_REPLY     = 0x0002,
	ARP_OP_RARPREQ   = 0x0003, /* Reverse ARP Request */
	ARP_OP_RARPREPLY = 0x0004, /* Reverse ARP Reply */
} arp_opcode;

struct arp_frame {
	unsigned short htype; /* hardware type */
	unsigned short ptype; /* protocol type */
	unsigned char hlen; /* hardware address length */
	unsigned char plen; /* protocol address length */
	unsigned short opecode; /* operation code */
	unsigned char host_mac[6];
	unsigned char host_ipaddr[4];
	unsigned char target_mac[6];
	unsigned char target_ipaddr[4];
} __attribute__((packed));

extern void arp_cache_clear(void);
extern int arp_search(struct net_device *dev, const unsigned char *target_ipaddr, unsigned char *target_mac);

extern int arp_proc(struct net_device *dev, struct net_packet *pkt);
#endif
