#ifndef _HERMIT_UDP_H_
#define _HERMIT_UDP_H_

struct ip_pseudo_header {
	unsigned char dst_ipaddr[4];
	unsigned char src_ipaddr[4];
	unsigned char zero;
	unsigned char protocol;
	unsigned short data_len;
} __attribute__((packed));

struct udp_frame {
	unsigned short src_port;
	unsigned short dst_port;
	unsigned short data_len;
	unsigned short checksum;
} __attribute__((packed));

extern int udp_send(struct net_device *dev, const unsigned char *dst_ipaddr, const unsigned short dst_port, const unsigned short src_port, struct net_packet *pkt);
extern int udp_proc(struct net_device *dev, const unsigned char *src_ipaddr, const unsigned short dst_port, unsigned short *src_port, struct net_packet *pkt);
#endif
