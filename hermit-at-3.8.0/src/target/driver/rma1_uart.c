#include <autoconf.h>
#include <hermit.h>
#include <herrno.h>
#include <arch/system.h>
#include <io.h>
#include <htypes.h>
#include <string.h>

extern int rma1_uart_request_port(int port);

#define INVALID_CONSOLE (-1)
static int cur_console = INVALID_CONSOLE;

struct rma1_uart {
	char *name;
	addr_t base;
};

struct rma1_uart uart[] = {
	{ .name = "ttySC0", .base = SCIFA0_BASE_ADDR, },
	{ .name = "ttySC1", .base = SCIFA1_BASE_ADDR, },
	{ .name = "ttySC2", .base = SCIFA2_BASE_ADDR, },
	{ .name = "ttySC3", .base = SCIFA3_BASE_ADDR, },
	{ .name = "ttySC4", .base = SCIFA4_BASE_ADDR, },
	{ .name = "ttySC5", .base = SCIFA5_BASE_ADDR, },
	{ .name = "ttySC6", .base = SCIFA6_BASE_ADDR, },
	{ .name = "ttySC7", .base = SCIFA7_BASE_ADDR, },
	{ .name = "ttySC8", .base = SCIFB_BASE_ADDR, },
};

static void rma1_setup_uart(int port)
{
	write16(uart[port].base + SCIF_SCR, 0x0030);
	write16(uart[port].base + SCIF_SMR, 0);

	/* reset FIFO */
	write16(uart[port].base + SCIF_FCR, 0x0006);
	write16(uart[port].base + SCIF_FCR, 0);

	/* set baudrate 115200 */
	write8(uart[port].base + SCIF_BRR, 0x1a);
}

int change_console(char *console)
{
	int i;

	for (i=0; i<ARRAY_SIZE(uart); i++) {
		if (strncmp(console, uart[i].name, strlen(uart[i].name)) == 0
		    && rma1_uart_request_port(i) == 0) {
			cur_console = i;
			if (uart[i].base)
				rma1_setup_uart(i);
			return 0;
		}
	}

	cur_console = INVALID_CONSOLE;

	return -H_EINVAL;
}

int arch_getchar(unsigned long timeout)
{
	u32 frdr;
	int ch;

	if (cur_console == INVALID_CONSOLE)
		return -H_EINVAL;

	if (cur_console == 8)
		/* SCIFB */
		frdr = SCIFB_FRDR;
	else
		/* SCIFA */
		frdr = SCIFA_FRDR;

	while (!(read16(uart[cur_console].base + SCIF_SSR) &
		 (SSR_RDF | SSR_DR)));

	ch = (int)read8(uart[cur_console].base + frdr);
	read16(uart[cur_console].base + SCIF_SSR);
	write16(uart[cur_console].base + SCIF_SSR, 0x00fc);

	return ch;
}

int arch_putchar(int c, unsigned long timeout)
{
	u32 ftdr, fdr;
	u32 shift;
	u16 count, full;
	u16 __maybe_unused mask;

	if (cur_console == INVALID_CONSOLE)
		return -H_EINVAL;

	if (cur_console == 8) {
		/* SCIFB */
		ftdr = SCIFB_FTDR;
		fdr = SCIFB_TFDR;
		shift = 0;
		full = SCIFB_FDR_FULL;
		mask = SCIFB_FDR_MASK;
	} else {
		/* SCIFA */
		ftdr = SCIFA_FTDR;
		fdr = SCIFA_FDR;
		shift = 8;
		full = SCIFA_FDR_FULL;
		mask = SCIFA_FDR_MASK;
	}

	/* waiting for fifo not full */
	do {
		count = read16(uart[cur_console].base + fdr) >> shift;
	} while (count >= full);

	write8(uart[cur_console].base + ftdr, c);

#if defined(ENABLE_WAIT_FOR_TX_COMPLETE)
	while ((read16(uart[cur_console].base + fdr) >> shift) & mask);
#endif
	return 0;
}
