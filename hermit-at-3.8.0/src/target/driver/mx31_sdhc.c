#define CORE_NAME "mx3_sdhc"

#include <autoconf.h>
#if defined(CONFIG_MMCSD_DEBUG)
#define TRACE_DEBUG
#define DEBUG
#endif
#include <hermit.h>
#include <herrno.h>
#include <platform.h>
#include <delay.h>
#include <arch/ioregs.h>
#include <mmcsd.h>

static int bus_width;

static void sdhc_clock_start(struct mmc_host *host)
{
	u32 val;

	val = read32(host->base + SDHC(STR_STP_CLK));
	val &= ~0x3;
	val |= 0x2;
	write32(host->base + SDHC(STR_STP_CLK), val);

	do {
		val = read32(host->base + SDHC(STATUS));
	} while (!(val & 0x100));
}

static void sdhc_clock_stop(struct mmc_host *host)
{
	u32 val;

	val = read32(host->base + SDHC(STR_STP_CLK));
	val &= ~0x3;
	val |= 0x1;
	write32(host->base + SDHC(STR_STP_CLK), val);

	do {
		val = read32(host->base + SDHC(STATUS));
	} while (val & 0x100);
}

static int sdhc_set_clock(struct mmc_host *host, u32 clock)
{
	u32 val;

	trace();

	if (clock) {
		/* Max speed: 20MHz */
		val = 0x0002;
	} else {
		/* Min speed: 117KHz */
		val = 0x010f;
	}
	write32(host->base + SDHC(CLK_RATE), val);

	return 0;
}

static int sdhc_pio_transfer_data(struct mmc_host *host,
				   struct mmc_command *cmd)
{
	u32 *buffer = (u32 *)cmd->data->buf;
	int i;

	for (i=0; i<cmd->data->blocks; i++) {
		u32 blksz = cmd->data->blksz;

		while (blksz) {
			while (!(read32(host->base + SDHC(STATUS)) & 0x880));

			*(buffer++) = read32(host->base + SDHC(BUFFER_ACCESS));
			blksz -= 4;
		}
	}
	write32(host->base + SDHC(STATUS), 0x800);

	return 0;
}

static int sdhc_do_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
	u32 mode;
	u32 stat;
	int i;

	sdhc_clock_start(host);

	mode = read32(host->base + SDHC(CMD_DAT_CONT)) & ~0x39f;

	if (cmd->opcode == MMC_GO_IDLE_STATE) {
		mode |= 0x80;
		bus_width = 1;
	}

	if (!(cmd->flags & MMC_RSP_PRESENT))
		mode |= 0x0;
	else if (cmd->flags & MMC_RSP_136)
		mode |= 0x2;
	else if (cmd->flags & MMC_RSP_CRC)
		mode |= 0x1;
	else
		mode |= 0x3;

	if (bus_width == 4)
		mode |= 0x200;

	if (cmd->data) {
		mode |= 0x8;
		if (cmd->data->flags & MMC_DATA_WRITE)
			mode |= 0x10;

		write32(host->base + SDHC(BLK_LEN), cmd->data->blksz);
		write32(host->base + SDHC(NOB), cmd->data->blocks);
	}

	write32(host->base + SDHC(STATUS), 0xffffffff);
	write32(host->base + SDHC(CMD), cmd->opcode);
	write32(host->base + SDHC(ARG), cmd->arg);
	if (cmd->debug)
		dev_dbg("XFERTYP: %p\n", mode);

	write32(host->base + SDHC(CMD_DAT_CONT), mode);

	for (i=1000000; i; i--) {
		stat = read32(host->base + SDHC(STATUS));
		if (stat & 0x2000)
			break;
		udelay(1);
	}

	if ((i <= 0) || (stat & 0x3)) {
		if (cmd->opcode != SD_SEND_IF_COND)
			dev_err("CMD:%d timedout. (status = %p)\n",
				cmd->opcode, stat, i);
		sdhc_clock_stop(host);
		return -H_EIO;
	}

	if (cmd->flags & MMC_RSP_136) {
		for (i=0; i<4; i++) {
			u32 rsp[2];
			rsp[0] = read32(host->base + SDHC(RES_FIFO)) & 0xffff;
			rsp[1] = read32(host->base + SDHC(RES_FIFO)) & 0xffff;

			cmd->resp[i] = (rsp[0] << 16) | rsp[1];
		}
	} else if (cmd->flags & MMC_RSP_PRESENT) {
		u32 rsp[3];
		rsp[0] = read32(host->base + SDHC(RES_FIFO)) & 0xffff;
		rsp[1] = read32(host->base + SDHC(RES_FIFO)) & 0xffff;
		rsp[2] = read32(host->base + SDHC(RES_FIFO)) & 0xffff;
		cmd->resp[0] = (rsp[0] << 24) | (rsp[1] << 8) | (rsp[2] >> 8);
	}

	if (cmd->debug)
		dev_dbg("RESP: %p, %p, %p, %p\n",
			cmd->resp[0], cmd->resp[1],
			cmd->resp[2], cmd->resp[3]);

	if (cmd->data) {
		sdhc_pio_transfer_data(host, cmd);

		if (cmd->data->stop) {
			int ret;
			ret = sdhc_do_cmd(host, cmd->data->stop);
			if (ret) {
				sdhc_clock_stop(host);
				return ret;
			}
		}
	}

	sdhc_clock_stop(host);

	return 0;
}

static int sdhc_get_card_detect(struct mmc_host *host)
{
	trace();

	return !mxc_get_gpio(GPIO_PIN(xSD1_CD));
}

static int sdhc_set_bus_width(struct mmc_host *host, u32 width)
{
	trace();

	bus_width = width;

	return 0;
}

static void sdhc_reset(struct mmc_host *host)
{
	trace();

	write32(host->base + SDHC(STR_STP_CLK), 0x8);
	write32(host->base + SDHC(STR_STP_CLK), 0x9);
	write32(host->base + SDHC(STR_STP_CLK), 0x1);
	write32(host->base + SDHC(STR_STP_CLK), 0x1);
	write32(host->base + SDHC(STR_STP_CLK), 0x1);
	write32(host->base + SDHC(STR_STP_CLK), 0x1);
	write32(host->base + SDHC(STR_STP_CLK), 0x1);
	write32(host->base + SDHC(STR_STP_CLK), 0x1);
	write32(host->base + SDHC(STR_STP_CLK), 0x1);
	write32(host->base + SDHC(STR_STP_CLK), 0x1);

	sdhc_set_clock(host, 0);

	write32(host->base + SDHC(RES_TO), 0xff);
	write32(host->base + SDHC(READ_TO), 0xffff);
	write32(host->base + SDHC(BLK_LEN), 512);
	write32(host->base + SDHC(NOB), 1);
}

static int sdhc_init(struct mmc_host *host)
{
	trace();

	sdhc_reset(host);

	return 0;
}

static void sdhc_exit(struct mmc_host *host)
{
	trace();

	sdhc_clock_stop(host);
	sdhc_reset(host);
}

static struct mmc_host_ops mx3_sdhc_ops = {
	.get_cd = sdhc_get_card_detect,
	.do_cmd = sdhc_do_cmd,
	.set_clock = sdhc_set_clock,
	.set_bus_width = sdhc_set_bus_width,
};

static struct mmc_host mx3_sdhc = {
	.init = sdhc_init,
	.exit = sdhc_exit,
	.ops = &mx3_sdhc_ops,
};

static int mx3_mmcsd_probe(struct mmcsd_device *dev, int port)
{
	trace();

	if (port != 0)
		return -H_EINVAL;

	mxc_set_mux(MUX_PIN(SD1_CLK),	MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(SD1_CMD),	MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(SD1_DATA0),	MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(SD1_DATA1),	MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(SD1_DATA2),	MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(SD1_DATA3),	MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(xSD1_CD),	MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_gpio(GPIO_PIN(xSD1_CD), GPIO_INPUT, 0);

	dev->host = &mx3_sdhc;
	dev->host->base = SDHC1_BASE_ADDR;

	return 0;
}

static void mx3_mmcsd_remove(struct mmcsd_device *dev)
{
	trace();
}

struct mmcsd_device mx3_mmcsd = {
	.probe = mx3_mmcsd_probe,
	.remove = mx3_mmcsd_remove,
};
