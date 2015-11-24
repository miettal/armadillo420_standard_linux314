#ifndef _HERMIT_IP_H_
#define _HERMIT_IP_H_

#define IP_VERSION 4
#define LEN_IP_Header 20

typedef enum _ip_proc_mode {
	ICMP_WAIT,
	TCP_WAIT,
	UDP_WAIT,
} ip_proc_mode;

struct ip_frame {
	unsigned char header_len:4,
		      version:4;
	unsigned char diffserv;
	unsigned short data_len;
	unsigned short id;
	unsigned short flag;
#define IP_CE		0x8000		/* Flag: "Congestion"		*/
#define IP_DF		0x4000		/* Flag: "Don't Fragment"	*/
#define IP_MF		0x2000		/* Flag: "More Fragments"	*/
#define IP_OFFSET	0x1FFF		/* "Fragment Offset" part	*/

	unsigned char  ttl; /* time to live */
	unsigned char  protocol;
	unsigned short checksum;
	unsigned char  src_ipaddr[4];
	unsigned char  dst_ipaddr[4];
} __attribute__((packed));

typedef enum _ip_protocol_id {
	IP_PROTOCOL_ICMP = 0x01,
	IP_PROTOCOL_TCP  = 0x06,
	IP_PROTOCOL_UDP  = 0x11,
} ip_protocol_id;

extern int ip_send(struct net_device *dev, const unsigned char *dst_ipaddr, const unsigned char protocol, struct net_packet *pkt);

extern int ip_proc(struct net_device *dev, const int mode, struct net_packet *pkt);

#endif
