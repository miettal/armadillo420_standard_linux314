/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#if !defined(WIN32)
#include <unistd.h>
#include <poll.h>
#include <termios.h>
#else
#include <io.h>
#endif







#if !defined(WIN32)
#include "eth.h"
#include "ethutil.h"
#else
#include "console.h"
#define snprintf _snprintf
#endif

#include "options.h"
#include "serial.h"
#include "target.h"
#include "util.h"
#include "console.h"



#define MAX_COMMAND_LENGTH 128

#define ETHERNET_MTU 65536
/* 
 * FIXME: We don't like this ifdef. let me know if you have any idea
 * what we should do!
 */
#ifdef AJ_FIRMUPDATE_SUPPORT
#define SERIAL_MTU 65536
#else
#define SERIAL_MTU 4096
#endif

static const char TM_ETHERNET[] = "ethernet";
static const char TM_SERIAL[] = "serial";

#if !defined(WIN32)
/* ethernet helper functions */
static int target_get_mac(target_context_t *tc);
static int target_negotiate_mac(target_context_t *tc);
static int target_set_mac(target_context_t *tc, const unsigned char *mac);
#endif

#ifndef WIN32
static void target_write_ethernet(target_context_t *tc,
				  const void *data, size_t count);
#endif
static void target_write_serial(target_context_t *tc,
				const void *data, size_t count);

target_context_t *target_open(const char *port, const char *netif)
{
	target_context_t *tc;

	assert(port);

	tc = zmalloc(sizeof (target_context_t));
#ifndef WIN32
	tc->portfd = -1;
#else
	tc->portfd = INVALID_HANDLE_VALUE;
#endif
	tc->sockfd = -1;
	tc->medium = TM_SERIAL;
	tc->mtu = SERIAL_MTU;
	tc->write = target_write_serial;

	tc->portfd = open_port(port);
	if(tc->portfd < 0)
		goto failure;

#if defined(WIN32)
	if(tc->portfd == INVALID_HANDLE_VALUE)
		goto failure;
#endif

#ifndef WIN32
	if (netif && (eth_open(tc, netif) < 0))
		goto failure;
#endif
	return tc;

failure:
	target_close(tc);
	return NULL;
}

int target_connect(target_context_t *tc)
{
	char buf [LINE_LENGTH];
	char str[256];

	assert(tc);
	if (target_ping(tc) < 0)
		return -1;
	msg("target_connect: ping successful.\n");

	/* write 'version' command to get version */
	target_write_command(tc, "version");

	sprintf(str, "target: %s", target_gets(tc, buf, sizeof buf));
	msg(str);

	if(target_confirm_response(tc) == -1){
		return -1;
	}

#if !defined(WIN32)
	/* switch to ethernet if possible and desired */
	if (opt_ethernet){
		if(target_set_medium(tc, "ethernet") == -1){
			return -1;
		}
	}
#endif

	return 0;
}

void target_close(target_context_t *tc)
{
	assert(tc);
#ifndef WIN32
	close(tc->portfd);
#else
	CloseHandle(tc->portfd);
#endif
	close(tc->sockfd);
	free(tc);
}

#ifndef WIN32
static int target_get_mac(target_context_t *tc)
{
	char buf [LINE_LENGTH];

	/* read MAC address from target */
	msg("getting target MAC\n");
	target_write_command(tc, "mac");
	target_gets(tc, buf, sizeof buf);
	if(target_confirm_response(tc) == -1){
		return -1;
	}

	/* and parse it */
	msgf("target-provided remote MAC: %s", buf);
	if (parsemac(tc->remote_mac, buf)){
		panicf("can't parse target-provided remote MAC: %s", buf);
		return -1;
	}
	if (opt_verbose) {
		msg("target-provided remote MAC (parsed): ");
		printmac(stdout, tc->remote_mac);
		msg("\n");
	}
	return 0;
}

static int target_negotiate_mac(target_context_t *tc)
{
	unsigned char bogus_mac_1[6] = { 0, 0, 0, 0, 0, 0 };
	unsigned char bogus_mac_2[6] = { 255, 255, 255, 255, 255, 255 };

	assert(opt_ethernet);
	if (opt_remote_mac){
		if(target_set_mac(tc, opt_remote_mac_bytes) == -1){
			return -1;
		}
	}else{
		if(target_get_mac(tc) == -1){
			return -1;
		}
	}

	/* check MAC validity */
	if (!memcmp(tc->remote_mac, bogus_mac_1, 6) ||
	    !memcmp(tc->remote_mac, bogus_mac_2, 6)){
		panic("target MAC is invalid (need --remote-mac)");
		return -1;
	}
	return 0;
}

static int target_set_mac(target_context_t *tc, const unsigned char *mac)
{
	char buf [CMDBUF_LENGTH];

	/* set MAC address on target */
	strcpy(buf, "mac ");
	sprintmac(buf + 4, mac);
	msgf("setting target MAC to `%s'\n", buf + 4);
	target_write_command(tc, buf);
	if(target_confirm_response(tc) == -1){
		return -1;
	}
	
	/* update our local copy on success */
	memcpy(tc->remote_mac, mac, 6);
	return 0;
}
#endif

int target_set_medium(target_context_t *tc, const char *medium)
{
	char cmdbuf [CMDBUF_LENGTH];

	msgf("setting medium to %s\n", medium);

#ifndef WIN32
	/* if medium is Ethernet, negotiate remote MAC before switching */
	if (!strcmp(medium, TM_ETHERNET))
		if(target_negotiate_mac(tc) == -1){
			return -1;
		}
#endif

	snprintf(cmdbuf, sizeof cmdbuf, "medium %s", medium);
	target_write_command(tc, cmdbuf);
	if(target_confirm_response(tc) == -1){
		return -1;
	}
	if (!strcmp(medium, TM_ETHERNET)) {
		tc->mtu = ETHERNET_MTU;
#ifndef WIN32
		tc->write = target_write_ethernet;
#endif
	} else if (!strcmp(medium, TM_SERIAL)) {
		tc->mtu = SERIAL_MTU;
		tc->write = target_write_serial;
	} else{
		panicf("unknown medium: %s", medium);
		return -1;
	}
	tc->medium = medium;
	return 0;
}

int target_confirm_response(target_context_t *tc)
{
	if (target_get_response(tc)){
		panic("target error, can't continue.");
		return -1;
	}
	return 0;
}

int target_get_response(target_context_t *tc)
{
	char buf [LINE_LENGTH];

	assert(tc);
	switch (ok_or_ng(target_gets(tc, buf, sizeof buf))) {
	case 1:
		msg("target: +OK\n");
		break;
	case 2:
		msg("target: +GO\n");
		break;
	case 0:
#ifndef WIN32
		warn("target didn't respond with +OK or -NG\n");
                warnf("unknown response `%s'\n", buf);
#endif
		return -1;
	case -1:
		/* warning message is generated by target_gets() */
		return -1;
	default:
		assert(0);
		return -1;
	}
	return 0;
}

char *target_gets(target_context_t *tc, char *s, int size)
{
	unsigned char c;
	char *p = s;

	assert(tc);
	assert(s);
	assert(size >= 0);
	if (!size--)
		return s;
	while (size--) {
		ssize_t nread = xread(tc->portfd, &c, 1);
		if(nread == ERROR_VALUE) break;
		if(nread == 0) break; //timeout

		if (c == '\r')
			++size;
		else
			*p++ = c;
		if (c == '\n')
			break;
	}
	*p = '\0';

	/* report error lines */
	if (ok_or_ng(s) < 0)
		warnf("from target: %s", s);
	return s;
}

void target_per_line(target_context_t *tc, iter_func_t *func, void *arg)
{
	char buf [LINE_LENGTH];

	assert(tc);
	assert(func);
	while (1) {
		target_gets(tc, buf, sizeof buf);	/* XXX handle error */
		msgf("read line from target: %s", buf);
		if (ok_or_ng(buf))
			break;
		if (func(buf, arg) < 0)
			break;
	}
}

/*
 * Try to put the target-side loader into batch mode; make sure it's
 * responding.  This is the only target function which times out.
 */
int target_ping(target_context_t *tc)
{
#define BUFLEN 127
	char buf [BUFLEN+1];	/* arbitrary size */
	char *ptr;
	time_t now, then;
#ifndef WIN32
	struct pollfd pfd;
	int ret;
#endif

	/* buf is always null-terminated */
	memset(buf, 0, sizeof buf);

	/* flush I/O on serial port */
	assert(tc);
#ifndef WIN32
	assert(tc->portfd >= 0);
	if (tcflush(tc->portfd, TCIOFLUSH) < 0)
		perror_xe("tcflush() on serial fd");
#else
	assert(tc->portfd != INVALID_HANDLE_VALUE);
	if (!FlushFileBuffers(tc->portfd))
		perror_xe("FlushFileBuffers() on serial fd");
#endif

	/* couple-second timeout */
	now = time(NULL);
	then = now + 2;

	/* write a '~' (this puts the loader into batch mode) */
#ifndef WIN32
	memset(&pfd, 0, sizeof pfd);
	pfd.fd = tc->portfd;
	pfd.events = POLLOUT;
	ret = poll(&pfd, 1, 1000 * (int)(then - now));
	if (ret < 0)
		perror_xe("poll() on serial fd");
	if (!ret)
		goto nobody_home;
#endif
	buf[0] = '~';
	xwrite(tc->portfd, buf, 1);

	/* wait for "+OK" response */
#ifndef WIN32
	pfd.events = POLLIN;
#endif
	for (ptr = buf; ptr - buf < BUFLEN; /* nada */) {
		char *pos;
		ssize_t nread;
		now = time(NULL);
		if (now >= then)
			goto nobody_home;
#ifndef WIN32
		ret = poll(&pfd, 1, 1000 * (int)(then - now));
		if (ret < 0)
			perror_xe("poll() on serial fd");
		if (!ret)
			goto nobody_home;
#endif

		nread = xread(tc->portfd, ptr++, 1);
		if(nread == ERROR_VALUE) goto nobody_home;

		pos = strstr(buf, "+OK\r\n");
		if (pos && pos[5] == '\0')
			return 0;
	}
nobody_home:
	return -1;
}

int target_retry_command_and_data(target_context_t *tc,
				   const char *command,
				   const void *data, size_t count)
{
	int retries = 3;

	while (retries--) {
		msgf("trying command/data (retries = %d)\n", retries);
		target_write_command(tc, command);
		if (target_get_response(tc))
			continue;
		target_write_data(tc, data, count);
		if (target_get_response(tc) == 0)
			return 0;
	}
	panic("command/data write retry count exceeded.");
	return -1;
}

void target_write_command(target_context_t *tc, const char *command)
{
	char buf [MAX_COMMAND_LENGTH + 1];
	size_t len;

	assert(tc);
	assert(command);
	assert(strlen(command) < sizeof buf - 2);

	/* tack '\n' onto the end of the command */
	len = strlen(command);
	memcpy(buf, command, len);
	buf[len++] = '\n';
	buf[len] = '\0';
	msgf("writing command: %s", buf);
	assert(tc->write);
	tc->write(tc, buf, len);
}

void target_write_data(target_context_t *tc, const void *data, size_t count)
{
	assert(tc);
	assert(data);

	assert(tc->write);
	tc->write(tc, data, count);
	target_add_progress(tc, count);
}

#ifndef WIN32
static void target_write_ethernet(target_context_t *tc,
				  const void *data, size_t count)
{
	size_t remain, nsent;

	assert(tc->mtu == ETHERNET_MTU);
	assert(count <= tc->mtu);
	for (remain = count; remain; remain -= nsent) {
		char c;
		nsent = remain;
		if (nsent > ETH_DOWNLOAD_BLOCK)
			nsent = ETH_DOWNLOAD_BLOCK;
		eth_send_frame(tc, data, nsent, 1);
		/* wait for ack */
		xread(tc->portfd, &c, 1);
		assert(c == 'K');
		data += nsent;
	}
}
#endif

static void target_write_serial(target_context_t *tc,
				const void *data, size_t count)
{
	assert(tc->mtu == SERIAL_MTU);
	assert(count <= tc->mtu);
	xawrite(tc->portfd, data, count);
#ifndef WIN32
	tcdrain(tc->portfd);
#endif
}

void target_add_progress(target_context_t *tc, size_t progress)
{
	char str[256];
	
	assert(tc);
	if (!tc->goal)
		return;
	tc->progress += progress;
	assert(tc->progress <= tc->goal);
	if (tc->progress >= tc->goal) {

		sprintf(str, "%s: completed 0x%08zx (%zu) bytes.          \n",
			tc->medium, tc->goal, tc->goal);
		console_msg(str);

		tc->goal = 0;
		tc->progress = 0;
	} else {
		sprintf(str, "%s: 0x%08zx (%zu) bytes of %zu...%c",
			tc->medium, tc->progress, tc->progress, tc->goal,
			opt_verbose ? '\n' : '\r');
		console_msg(str);
	}
}

void target_set_goal(target_context_t *tc, size_t goal)
{
	assert(tc);
	tc->goal = goal;
	tc->progress = 0;
	target_add_progress(tc, 0);
}

int ok_or_ng(const char *s)
{
	assert(s);
	if (!strncmp(s, "+OK", 3))
		return 1;
	if (!strncmp(s, "+GO", 3))
		return 2;
	if (!strncmp(s, "-NG", 3)){
	  console_msg(s);
		return -1;
	}
	return 0;
}

