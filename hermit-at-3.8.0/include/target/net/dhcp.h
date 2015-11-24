#ifndef _HERMIT_DHCP_H_
#define _HERMIT_DHCP_H_

#include <net/net.h>

#define BOOTP_SERVER_PORT 67
#define BOOTP_CLIENT_PORT 68

#define BOOTP_OPTION_LEN 308 /* 312 - 4 (magic) */

struct bootp_pkt {
	u8 op;
	u8 htype;
	u8 hlen;
	u8 hops;
	u32 xid;
	u16 secs;
	u16 flags;
	u8 client_ip[4];
	u8 your_ip[4];
	u8 server_ip[4];
	u8 relay_ip[4];
	u8 hw_addr[16];
	u8 server_name[64];
	u8 boot_file[128];
	u32 magic;
	u8 option[BOOTP_OPTION_LEN];
} __attribute__((packed));

extern int dhcp_get_ipaddr(struct net_device *dev, unsigned char *ipaddr);

#endif
