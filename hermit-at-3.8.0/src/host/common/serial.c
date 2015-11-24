/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <termios.h>
#include <unistd.h>
#endif

#include "options.h"
#include "serial.h"
#include "util.h"

#ifndef WIN32
/* need file scope so that we can restore state. */
static struct termios newtios, oldtios, contios;	/* terminal settings */
static int saved_portfd = -1;		/* serial port fd */
static int terminal_mode;		/* are we in terminal mode? */

/* restore serial tty state if possible */
static void reset_tty_atexit(void);
static void reset_tty_handler(int signal);
#endif

/* open the port */
#ifndef WIN32
int open_port(const char *portname)
#else
HANDLE open_port(const char *portname)
#endif
{
#ifndef WIN32
	struct sigaction sa;
	int portfd;
#else
	HANDLE portfd;
	DCB dcb;
	char port[128];
#endif

	msgf("opening serial port: %s\n", portname);

	/* open serial port */
#ifndef WIN32
	if ((portfd = open(portname, O_RDWR|O_NOCTTY)) < 0)
		perror_xe("open serial port");
#else
	sprintf(port, "\\\\.\\%s", portname);
	if ((portfd = CreateFile(port, GENERIC_READ|GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
			return INVALID_HANDLE_VALUE;
#endif
	
#ifndef WIN32
	/* get serial port params, save away */
	tcgetattr(portfd, &newtios);
	memcpy(&oldtios, &newtios, sizeof newtios);

	/* configure new values */
	cfmakeraw(&newtios);		/* see man page */
	newtios.c_iflag |= IGNPAR;	/* ignore parity on input */
	newtios.c_oflag &= ~(OPOST|ONLCR|OLCUC|OCRNL|ONOCR|ONLRET|OFILL);
	newtios.c_cflag = CS8|CLOCAL|CREAD;
	newtios.c_cc[VMIN] = 1;		/* block until 1 char received */
	newtios.c_cc[VTIME] = 0;	/* no inter-character timer */

	/* baudrate */
	cfsetospeed(&newtios, opt_baudrate->speed);
	cfsetispeed(&newtios, opt_baudrate->speed);

	/* register cleanup stuff  */
	atexit(reset_tty_atexit);
	memset(&sa, 0, sizeof sa);
	sa.sa_handler = reset_tty_handler;
	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	/* apply modified termios */
	saved_portfd = portfd;
	tcflush(portfd, TCIFLUSH);
	tcsetattr(portfd, TCSADRAIN, &newtios);
#else
	set_serial_timeout(portfd, TIMEOUT_NORMAL);

	GetCommState (portfd, &dcb);
	dcb.BaudRate = opt_baudrate->speed;
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	SetCommState (portfd, &dcb);
#endif
        return portfd;
}

//Add 06/03/07
#ifdef WIN32
int set_serial_timeout(HANDLE portfd, int msec){
	COMMTIMEOUTS timeouts;
	int ret;

	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.ReadTotalTimeoutConstant = msec;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = msec;

	ret = SetCommTimeouts (portfd, &timeouts);
	return ret;
}
#endif

#ifndef WIN32
void close_port(int portfd)
{
	assert(portfd == saved_portfd);
	tcsetattr(portfd, TCSANOW, &oldtios);
	close(portfd);
	saved_portfd = -1;
}
#else
void close_port(HANDLE portfd)
{
	CloseHandle(portfd);
}
#endif

#ifndef WIN32
/* cleanup atexit handler */
static void reset_tty_atexit(void)
{
	if (saved_portfd != -1) {
		tcsetattr(saved_portfd, TCSANOW, &oldtios);
		if (terminal_mode)
			tcsetattr(STDIN_FILENO, TCSANOW, &contios);
	}
}

/* cleanup signal handler */
static void reset_tty_handler(int signal)
{
	if (saved_portfd != -1) {
		tcsetattr(saved_portfd, TCSANOW, &oldtios);
		if (terminal_mode)
			tcsetattr(STDIN_FILENO, TCSANOW, &contios);
	}
	_exit(EXIT_FAILURE);
}

/* enter terminal mode */
void serial_terminal(void)
{
	struct termios tio;
		
	printf("Entering terminal mode -- interrupt to exit\n");
	assert(saved_portfd >= 0);
	tcgetattr(STDIN_FILENO, &contios);
	terminal_mode = 1;

	/* set input mode (non-canonical, no echo,...) */
	memcpy(&tio, &contios, sizeof tio);
	tio.c_lflag = ISIG;
	tio.c_cc[VTIME] = 0;	/* inter-character timer unused */
	tio.c_cc[VMIN] = 1;	/* blocking read until 1 char received */
	tcsetattr(STDIN_FILENO, TCSANOW, &tio);

	while (1) {
		fd_set fds;
		int retval;
		unsigned char c;
			
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		FD_SET(saved_portfd, &fds);

		retval = select(saved_portfd + 1, &fds, NULL, NULL, NULL);
		if (FD_ISSET(saved_portfd, &fds)) {
			if (read(saved_portfd, &c, 1) == 1)
				putchar(c);
		}
		if (FD_ISSET(STDIN_FILENO, &fds)) {
			if (read(STDIN_FILENO, &c, 1) == 1)
				xwrite(saved_portfd, &c, 1);
		}
		fflush(NULL);
	}
}
#endif

