/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#if !defined(WIN32)
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#else
//#include <winsock.h>
#include <winsock2.h>
#endif

#include <stdlib.h>
#include <string.h>



#include "eth.h"
#include "ethutil.h"
#include "options.h"
#include "util.h"

static int raw_socket;

void eth_raw_socket(void)
{
	/* open the socket */
	raw_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (raw_socket < 0)
		perror_xe("socket");
}

/* open a packet socket for access to the local ethernet */
int eth_open(target_context_t *tc, const char *netif)
{
	struct sockaddr_ll sa;
	struct ifreq ifr;
	int sock;

	assert(tc);
	assert(tc->sockfd == -1);

	/* raw socket should have been opened with root euid */
	assert(raw_socket > 0);
	sock = raw_socket;
	raw_socket = 0;

	msgf("binding to network interface: %s\n", netif);

	/* initialize low-level socket address */
	memset(&sa, 0, sizeof sa);
	sa.sll_family   = AF_PACKET;
	sa.sll_protocol = htons(ETH_P_ALL);
	sa.sll_ifindex  = if_nametoindex(netif);

	/* bind socket to name (socket address) */
	if (bind(sock, (struct sockaddr*) &sa, sizeof sa) < 0){
		perror_xe("bind");
		return -1;
	}

	/* get mac address of the interface; confirm Ethernet */
	memset(&ifr, 0, sizeof ifr);
	strncpy(ifr.ifr_name, netif, sizeof ifr.ifr_name);
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
		perror_xe("ioctl(SIOCGIFHWADDR)");
	if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER){
		panicf("%s is not an Ethernet interface", netif);
		return -1;
	}

	memcpy(tc->local_mac, ifr.ifr_hwaddr.sa_data, 6);
	if (opt_verbose) {
		msgf("MAC address for %s is ", netif);
		printmac(stderr, tc->local_mac);
		msg("\n");
	}

	tc->sockfd = sock;
	return 0;
}

/*
 * Write data onto the local ethernet.  We construct a raw frame
 * and send it via a packet socket.
 *
 * The standard Ethernet frame format is:
 * -- 6-byte destination MAC address
 * -- 6-byte source MAC address
 * -- 2-byte type (we make one up)
 *
 * We add a 2-byte data length field next, with most significant
 * byte first, followed by a 4-byte sequence number, again MSB
 * first.
 *
 * We also make sure the frame length is a multiple of 4 (not
 * really necessary), and is at least 64 bytes (the lower layers
 * probably do this for us anyway--you typically have to mess
 * with the hardware to transmit a runt frame).
 */
#define STD_HEADER_BYTES 14
#define OUR_HEADER_BYTES (STD_HEADER_BYTES + 6)

void eth_send_frame(target_context_t *tc, const void *buf,
		    size_t count, unsigned seq)
{
	unsigned char frame [ETH_DOWNLOAD_BLOCK + OUR_HEADER_BYTES];
	size_t size;

	assert(tc);
	assert(tc->sockfd >= 0);
	assert(buf);
	assert(count > 0);
	assert(count <= ETH_DOWNLOAD_BLOCK);

	/* construct standard Ethernet frame */
	memset(frame, 0, sizeof frame);
	memcpy(frame, tc->remote_mac, 6);
	memcpy(frame + 6, tc->local_mac, 6);
	frame[12] = '4';
	frame[13] = '2';

	/* add our packet header */
	frame[14] = count >> 8;
	frame[15] = count;
	frame[16] = seq >> 24;
	frame[17] = seq >> 16;
	frame[18] = seq >> 8;
	frame[19] = seq;
	memcpy(frame + OUR_HEADER_BYTES, buf, count);

	/* be extra careful */
	size = count + OUR_HEADER_BYTES;
	if (size < 64)
		size = 64;
	if (size & 3)
		size = (size + 3) & ~3;

	/* error could just be an incomplete write, but with packet
	   socket I imagine that's probably not a good thing. */
	if (write(tc->sockfd, frame, size) != size)
		perror_xe("write to packet socket");
}
 
/* close ethernet socket */
void eth_close(target_context_t *tc)
{
	assert(tc);
	assert(tc->sockfd >= 0);
	xclose(tc->sockfd);
	tc->sockfd = -1;
}

