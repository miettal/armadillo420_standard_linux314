#define CORE_NAME "rma1_mmc"

#include <autoconf.h>
#if defined(CONFIG_MMCSD_DEBUG)
#define TRACE_DEBUG
#define DEBUG
#endif
#include <hermit.h>
#include <herrno.h>
#include <platform.h>
#include <delay.h>
#include <arch/system.h>
#include <mmcsd.h>
#include <rma1_gpio.h>
#include <platform/pinmux.h>

extern int rma1_mmc_request_port(void);

static int bus_width = 1;

static void mmc_clock_control(struct mmc_host *host, int start)
{
	u32 val;
	int i;

	trace();

	/* Waiting for SD Bus busy to be cleared */
	for (i=0; read32(MMC(HOST_STS1)) & 0x80000000; i++) {
		if ((i & 0xfff) == 0)
			dev_dbg(" %s(%d) wait: %p\n", __func__, i, read32(MMC(HOST_STS1)));
	}

	val = read32(MMC(CLK_CTRL));
	if (start)
		val |= 0x01000000;
	else
		val &= ~0x01000000;
	write32(MMC(CLK_CTRL), val);
}
#define mmc_clock_start(host) mmc_clock_control(host, 1)
#define mmc_clock_stop(host) mmc_clock_control(host, 0)

static int mmc_set_clock(struct mmc_host *host, u32 clock)
{
	u32 val, tmp;
	int i;

	trace();

	/* Waiting for SD Bus busy to be cleared */
	for (i=0; read32(MMC(HOST_STS1)) & 0x80000000; i++) {
		if ((i & 0xfff) == 0)
			dev_dbg(" %s(%d) wait: %p\n", __func__, i, read32(MMC(HOST_STS1)));
	}

	tmp = read32(MMC(CLK_CTRL));
	write32(MMC(CLK_CTRL), tmp & ~(1 << 24));
	val = tmp & ~0x000f0000;

	if (clock)
		val |= (0x1 << 16);//25MHz
	else
		val |= (0x9 << 16);//96KHz

	write32(MMC(CLK_CTRL), val | (tmp & (1 << 24)));

	return 0;
}

static int mmc_pio_data_read(struct mmc_host *host,
			     struct mmc_command *cmd)
{
	u32 *buffer = (u32 *)cmd->data->buf;
	u32 stat;
	int i;
	int to;

	trace();

	for (i=0; i<cmd->data->blocks; i++) {
		int blksz = (int)cmd->data->blksz;

		for (to=1000000; to; to--) {
			stat = read32(MMC(INT));

			if (stat & (0xcf1f | (1 << 20)))
				break;
		}

		if (to == 0)
			return -H_EIO;

		if (stat & 0xcf1f) {
			do {
				write32(MMC(INT), 0x00000000);
				write32(MMC(CMD_CTRL), 1);
				write32(MMC(CMD_CTRL), 0);
			} while ((read32(MMC(HOST_STS1)) & 0x80000000));

			dev_err("DATA READ:%d error/timedout. (status = %p)\n",
				cmd->opcode, stat, i);

			return -H_EIO;
		}

		write32(MMC(INT), 0xffefffff);

		while (blksz > 0) {
			*(buffer++) = read32(MMC(DATA));
			blksz -= 4;
		}
	}

	while (!(read32(MMC(INT)) & (1 << 22)));
	write32(MMC(INT), 0xffbfffff);

	if (0){//dump
		int j;
		u8 *buffer = (u8 *)cmd->data->buf;
		for (j=0;j<512;j++) {
			hprintf("%b ", *(buffer++));
			if (j%16 == 15)hprintf("\n");
		}
		if (j%16 != 0)hprintf("\n");

	}
	return 0;
}

static int mmc_pio_data_write(struct mmc_host *host,
			      struct mmc_command *cmd)
{
	u32 *buffer = (u32 *)cmd->data->buf;
	u32 stat;
	int i;
	int to;

	trace();

	for (i=0; i<cmd->data->blocks; i++) {
		int blksz = (int)cmd->data->blksz;

		for (to=1000000; to; to--) {
			stat = read32(MMC(INT));

			if (stat & (0xcf1f | (1 << 21)))
				break;
		}

		if (to == 0)
			return -H_EIO;

		if (stat & 0xcf1f) {
			do {
				write32(MMC(INT), 0x00000000);
				write32(MMC(CMD_CTRL), 1);
				write32(MMC(CMD_CTRL), 0);
			} while ((read32(MMC(HOST_STS1)) & 0x80000000));

			dev_err("DATA WRITE:%d error/timedout. (status = %p)\n",
				cmd->opcode, stat, i);

			return -H_EIO;
		}

		write32(MMC(INT), 0xffdfffff);

		while (blksz > 0) {
			write32(MMC(DATA), *(buffer++));
			blksz -= 4;
		}
	}

	while (!(read32(MMC(INT)) & (1 << 23)));
	write32(MMC(INT), 0xff7fffff);

	return 0;
}

static int mmc_pio_transfer_data(struct mmc_host *host,
				 struct mmc_command *cmd)
{
	int ret;

	if (cmd->data->flags & MMC_DATA_READ)
		ret = mmc_pio_data_read(host, cmd);
	else
		ret = mmc_pio_data_write(host, cmd);

	return ret;
}

static int mmc_do_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
	u32 mode;
	int i;
	u32 stat;
	int ret = 0;

	trace();

	cmd->debug = 1;

	mmc_clock_start(host);

	if (cmd->debug)
		dev_dbg("CMD%d\n", cmd->opcode);

	mode = cmd->opcode << 24;

	if (bus_width == 1)
		mode |= 0x0;
	else if (bus_width == 4)
		mode |= 0x01;
	else if (bus_width == 8)
		mode |= 0x02;

	/* Response Type check */
	switch (mmc_resp_type(cmd)) {
	case MMC_RSP_NONE:
		mode |= 0 << 22;//CMD_SET_RTYP_NO;
		break;
	case MMC_RSP_R1:
	case MMC_RSP_R1B:
	case MMC_RSP_R3:
		mode |= 1 << 22;//CMD_SET_RTYP_6B;
		break;
	case MMC_RSP_R2:
		mode |= 2 << 22;//CMD_SET_RTYP_17B;
		break;
	default:
		dev_err("Unsupported response type.\n");
		break;
	}
	switch (cmd->opcode) {
	/* RBSY */
	case MMC_SWITCH:
	case MMC_STOP_TRANSMISSION:
	case MMC_SET_WRITE_PROT:
	case MMC_CLR_WRITE_PROT:
	case MMC_ERASE:
	case MMC_GEN_CMD:
		mode |= 1 << 21;//CMD_SET_RBSY;
		break;
	}
	/* DWEN */
	if (cmd->opcode == MMC_WRITE_BLOCK ||
	    cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK)
		mode |= 1 << 18;//CMD_SET_DWEN;
	/* CMLTE/CMD12EN */
	if (cmd->opcode == MMC_READ_MULTIPLE_BLOCK ||
	    cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK) {
		mode |= (1 << 17);//CMD_SET_CMLTE
		mode |= (1 << 16);//CMD_SET_CMD12EN;
	}
	/* RIDXC[1:0] check bits */
	if (cmd->opcode == MMC_SEND_OP_COND || cmd->opcode == MMC_ALL_SEND_CID ||
	    cmd->opcode == MMC_SEND_CSD || cmd->opcode == MMC_SEND_CID)
		mode |= 1 << 14;
	/* RCRC7C[1:0] check bits */
	if (cmd->opcode == MMC_SEND_OP_COND)
		mode |= 1 << 12;//CMD_SET_CRC7C_BITS;
	/* RCRC7C[1:0] internal CRC7 */
	if (cmd->opcode == MMC_ALL_SEND_CID ||
	    cmd->opcode == MMC_SEND_CSD ||
	    cmd->opcode == MMC_SEND_CID)
		mode |= 2 << 12;//CMD_SET_CRC7C_INTERNAL;

	if (cmd->data) {
		mode |= (1 << 19);

		if (!(cmd->data->flags & MMC_DATA_READ))
			mode |= 1 << 18;

		if (cmd->data->blocks > 1) {
			mode |= (1 << 17);

			write32(MMC(BLOCK_SET), (cmd->data->blocks << 16) |
				cmd->data->blksz);
		} else {
			write32(MMC(BLOCK_SET), cmd->data->blksz);
		}
	}

	if (cmd->debug)
		dev_dbg("XFERTYP: %p\n", mode);

#if 0
	while ((read32(MMC(HOST_STS1)) & 0x80000000)) {
		hprintf(" * CMD%d wait: %p\n", cmd->opcode, read32(MMC(HOST_STS1)));
		write32(MMC(INT), 0x00000000);
		write32(MMC(CMD_CTRL), 1);
		write32(MMC(CMD_CTRL), 0);
	}
#endif

	/* status clear */
	write32(MMC(INT), 0xd80430c0);

	write32(MMC(ARG), cmd->arg);
	write32(MMC(CMD_SET), mode);

	for (i=1000000; i; i--) {
		stat = read32(MMC(INT));
		if (stat & 0x1cf1f)
			break;
		udelay(1);
	}

	if (stat & 0xcf1f) {
		do {
			write32(MMC(INT), 0x00000000);
			write32(MMC(CMD_CTRL), 1);
			write32(MMC(CMD_CTRL), 0);
		} while ((read32(MMC(HOST_STS1)) & 0x80000000));

		if (host->card.type != MMC_TYPE_UNKNOWN) {
			dev_err("CMD:%d error/timedout. (status = %p)\n",
				cmd->opcode, stat, i);
		}
		return -H_EIO;
	}
	write32(MMC(INT), 0xfffcffff);

	if (cmd->flags & MMC_RSP_136) {
		cmd->resp[0] = read32(MMC(RESP3));
		cmd->resp[1] = read32(MMC(RESP2));
		cmd->resp[2] = read32(MMC(RESP1));
		cmd->resp[3] = read32(MMC(RESP0));
	} else {
		cmd->resp[0] = read32(MMC(RESP0));
	}

	if (cmd->debug)
		dev_dbg("RESP: %p, %p, %p, %p\n",
			cmd->resp[0], cmd->resp[1],
			cmd->resp[2], cmd->resp[3]);

	if (cmd->data) {
		ret = mmc_pio_transfer_data(host, cmd);

		if (cmd->data->stop) {
			int stop_opcode = cmd->data->stop->opcode;

			if (stop_opcode == 12) {
				cmd->resp[0] = read32(MMC(RESP_CMD12));
				while (!(read32(MMC(INT)) & (1 << 24)));
				write32(MMC(INT), 0xff8fffff);
				ret = 0;
			} else {
				ret = mmc_do_cmd(host, cmd->data->stop);
			}
			return ret;
		}
	}

	//mmc_clock_stop(host);

	return ret;
}

static int mmc_get_card_detect(struct mmc_host *host)
{
	trace();
	return 1;
}

static int mmc_set_bus_width(struct mmc_host *host, u32 width)
{
	trace();
	bus_width = width;
	return 0;
}

static void mmc_reset(struct mmc_host *host)
{
	trace();

	write32(MMC(VERSION), 0x80000000);
	mdelay(1);
	write32(MMC(VERSION), 0x00000000);
	write32(MMC(CLK_CTRL), 0x00092ff0);
	write32(MMC(BUF_ACC), 0x00010000);
}

static int mmc_init(struct mmc_host *host)
{
	trace();

	rma1_mmc_request_port();

	/* Chip Reset */
	gpio_direction_output(xMMC0_RST_B, 0);
	mdelay(10);
	gpio_set_value(xMMC0_RST_B, 1);

	cpg_clr_mstpcr(312);
	write32(0xe6058024, 0x8000);

	mmc_reset(host);

	mmc_set_clock(host, 0);
	mmc_set_bus_width(host, 1);

	return 0;
}

static void mmc_exit(struct mmc_host *host)
{
	trace();
}

static struct mmc_host_ops rma1_mmc_ops = {
	.get_cd = mmc_get_card_detect,
	.do_cmd = mmc_do_cmd,
	.set_clock = mmc_set_clock,
	.set_bus_width = mmc_set_bus_width,
};

struct mmc_host rma1_mmc = {
	.init = mmc_init,
	.exit = mmc_exit,
	.ops = &rma1_mmc_ops,
};
