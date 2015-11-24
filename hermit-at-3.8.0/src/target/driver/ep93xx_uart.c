#include <hermit.h>
#include <htypes.h>
#include <herrno.h>
#include <string.h>
#include <io.h>
#include <arch/ioregs.h>

static int cur_console = 1;

struct ep93xx_uart {
	char *name;
	addr_t fr;
	addr_t dr;
};

static struct ep93xx_uart uart[] = {
	{ .name = "none", },
	{ .name = "ttyAM0", .fr = UART1FR, .dr = UART1DR, },
	{ .name = "ttyAM1", .fr = UART2FR, .dr = UART2DR, },
	{ .name = "ttyAM2", .fr = UART3FR, .dr = UART3DR, },
};

int change_console(char *console)
{
	int i;

	for (i=0; i<ARRAY_SIZE(uart); i++) {
		if (strncmp(console, uart[i].name, strlen(uart[i].name)) == 0) {
			cur_console = i;
			return 0;
		}
	}

	change_console("ttyAM0");
	return -H_EINVAL;
}

int arch_getchar(unsigned long timeout)
{
	if (cur_console == 0)
		return 0;

	while (read32(uart[cur_console].fr) & UARTFR_RXFE);
	return read32(uart[cur_console].dr) & 0xff;
}

int arch_putchar(int c, unsigned long timeout)
{
	if (cur_console != 0) {
		while (read32(uart[cur_console].fr) & UARTFR_TXFF);
		write32(uart[cur_console].dr, c);
	}

	return 0;
}
