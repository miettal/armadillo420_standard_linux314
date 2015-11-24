#include <hermit.h>
#include <htypes.h>
#include <herrno.h>
#include <string.h>
#include <arch/ioregs.h>
#include <io.h>

static int cur_console = 1;

struct ns9750_uart {
	char *name;
	addr_t status_a;
	addr_t fifo_data;
};

static struct ns9750_uart uart[] = {
	[0] = {
		.name = "none",
		.status_a = 0,
		.fifo_data = 0,
	},
	[1] = {
		.name = "ttyAM0",
		.status_a = NS_SER_CHA_STATUS_A,
		.fifo_data = NS_SER_CHA_FIFO_DATA,
	},
	[2] = {
		.name = "ttyAM1",
		.status_a = NS_SER_CHB_STATUS_A,
		.fifo_data = NS_SER_CHB_FIFO_DATA,
	},
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
	static u32 data;
	static u8 rxfdb = 0;

	if (cur_console == 0)
		return 0;

	if (!rxfdb) {
		u32 status;
		while (1) {
			status = read32(NS9750_SER_PHYS_BASE +
					uart[cur_console].status_a);
			if (status & SER_STATUS_RRDY)
				break;
			if (status & SER_STATUS_RBC) {
				write32(NS9750_SER_PHYS_BASE +
					uart[cur_console].status_a,
					SER_STATUS_RBC);
			}
		}
		rxfdb = SER_STATUS_RXFDB(status);
		data = read32(NS9750_SER_PHYS_BASE +
			      uart[cur_console].fifo_data);
		if (!rxfdb)
			rxfdb = 4;
	} else {
		data >>= 8;
	}
	rxfdb--;
	return (data & 0xff);
}

int arch_putchar(int c, unsigned long timeout)
{
	u32 status;

	if (cur_console == 0)
		return 0;

	while (1) {
		status = read32(NS9750_SER_PHYS_BASE +
				uart[cur_console].status_a);
		if (status & SER_STATUS_TRDY)
			break;
	}
	write8(NS9750_SER_PHYS_BASE + uart[cur_console].fifo_data, c);

	return 0;
}
