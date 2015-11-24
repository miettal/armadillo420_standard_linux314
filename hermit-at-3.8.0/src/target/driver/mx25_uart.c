#include <autoconf.h>
#include <hermit.h>
#include <herrno.h>
#include <arch/system.h>
#include <arch/iomux.h>
#include <arch/clock.h>
#include <io.h>
#include <htypes.h>
#include <string.h>

extern int mx25_uart_post_setup(int uart);

static int cur_console = 2;

struct mx25_uart {
	char *name;
	addr_t base;
	u32 clk;
};

struct mx25_uart uart[] = {
	{ .name = "none", },
	{ .name = "ttymxc0", .base = UART1_BASE_ADDR, .clk = 46, },
	{ .name = "ttymxc1", .base = UART2_BASE_ADDR, .clk = 47, },
	{ .name = "ttymxc2", .base = UART3_BASE_ADDR, .clk = 48, },
	{ .name = "ttymxc3", .base = UART4_BASE_ADDR, .clk = 49, },
	{ .name = "ttymxc4", .base = UART5_BASE_ADDR, .clk = 50, },
};

static void mx25_setup_uart(int port)
{
	mx25_per_clock_enable(15);
	mx25_ipg_clock_enable(uart[port].clk);

	/* force DTE mode */
	write32(uart[port].base + UART_UFCR, 0x40);

	write32(uart[port].base + UART_UCR1, 0x0);
	write32(uart[port].base + UART_UCR2, 0x0);
	write32(uart[port].base + UART_UCR3, 0x704);
	write32(uart[port].base + UART_UCR4, 0x8000);
	write32(uart[port].base + UART_UESC, 0x2b);
	write32(uart[port].base + UART_UTIM, 0x0);
	write32(uart[port].base + UART_UBIR, 0x0);
	write32(uart[port].base + UART_UBMR, 0x0);
	write32(uart[port].base + UART_ONEMS, 0x0);
	write32(uart[port].base + UART_UTS, 0x0);

	write32(uart[port].base + UART_UCR3, 0x704);
	write32(uart[port].base + UART_UFCR, 0xac1);
	write32(uart[port].base + UART_ONEMS, 0x46a);

	/* Baud:115200 = 120M / (16 * ((0xc34 + 1) / (0x27 + 1)) */
	write32(uart[port].base + UART_UBIR, 0x2f);
	write32(uart[port].base + UART_UBMR, 0xc34);

	write32(uart[port].base + UART_UCR1, 0x1);
	write32(uart[port].base + UART_UCR2, 0x4027);
	write32(uart[port].base + UART_USR2, 0xffff);
	write32(uart[port].base + UART_USR1, 0xffff);
}

int change_console(char *console)
{
	int i;

	for (i=0; i<ARRAY_SIZE(uart); i++) {
		if (strncmp(console, uart[i].name, strlen(uart[i].name)) == 0 &&
		    mx25_uart_post_setup(i) == 0) {
			cur_console = i;
			if (uart[i].base)
				mx25_setup_uart(i);
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

	while (!(read32(uart[cur_console].base + UART_USR1) & 0x200));
	return read32(uart[cur_console].base + UART_URXD) & 0xff;
}

int arch_putchar(int c, unsigned long timeout)
{
	if (cur_console != 0) {
		while (!(read32(uart[cur_console].base + UART_USR2) & 0x4000));
		write32(uart[cur_console].base + UART_UTXD, c);
	}
	return 0;
}
