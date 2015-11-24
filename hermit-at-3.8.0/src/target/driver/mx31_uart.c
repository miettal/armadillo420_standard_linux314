#include <hermit.h>
#include <htypes.h>
#include <herrno.h>
#include <string.h>
#include <io.h>
#include <arch/ioregs.h>

static int cur_console = 1;

struct mx31_uart {
	char *name;
	addr_t base;
};

static struct mx31_uart uart[] = {
	{ .name = "none", },
	{ .name = "ttymxc0", .base = UART1_BASE_ADDR, },
	{ .name = "ttymxc1", .base = UART2_BASE_ADDR, },
	{ .name = "ttymxc2", .base = UART3_BASE_ADDR, },
	{ .name = "ttymxc3", .base = UART4_BASE_ADDR, },
	{ .name = "ttymxc4", .base = UART5_BASE_ADDR, },
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

	cur_console = 0;
	return -H_EINVAL;
}

int arch_getchar(unsigned long timeout)
{
	if (cur_console == 0)
		return 0;

	while (!(read32(uart[cur_console].base + USR1) & USR1_RRDY));
	return (read32(uart[cur_console].base + URXD) & 0xff);
}

int arch_putchar(int c, unsigned long timeout)
{
	if (cur_console != 0) {
		while (!(read32(uart[cur_console].base + USR2) & USR2_TXFE));
		write32(uart[cur_console].base + UTXD, c);
	}

	return 0;
}
