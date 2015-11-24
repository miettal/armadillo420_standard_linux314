#define CORE_NAME "rma1_sdhc"

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

extern int rma1_sdhc_request_port(int port);

#if defined(CONFIG_PLATFORM_ARMADILLO800EVA)
#define SDHI0_ENABLE_EXTERNAL_CD
#endif

static void sdhc_clock_start(struct mmc_host *host)
{
	u16 val;

	trace();
	/* Waiting for SD Bus busy to be cleared */
	while (!(read16(host->base + SDHI_INFO2) & 0x2000));
	val = read16(host->base + SDHI_CLK_CTRL);
	val |= 0x0300;
	write16(host->base + SDHI_CLK_CTRL, val);
}

static void __attribute__((unused)) sdhc_clock_stop(struct mmc_host *host)
{
	u32 val;

	trace();
	/* Waiting for SD Bus busy to be cleared */
	while (!(read16(host->base + SDHI_INFO2) & 0x2000));
	val = read16(host->base + SDHI_CLK_CTRL);
	val &= ~0x0300;
	write16(host->base + SDHI_CLK_CTRL, val);
}

static int sdhc_set_clock(struct mmc_host *host, u32 clock)
{
	u32 val;

	trace();

	/* Waiting for SD Bus busy to be cleared */
	while (!(read16(host->base + SDHI_INFO2) & 0x2000));

	val = read16(host->base + SDHI_CLK_CTRL);
	val &= ~0xff;
	if (clock)
		/* Max speed: 25MHz */
		val |= 0x01;
	else
		/* Min speed: 195KHz */
		val |= 0x80;
	write16(host->base + SDHI_CLK_CTRL, val);

	return 0;
}

#if defined(CONFIG_MMCSD_RMA1_SDHC_ERRATA_ENGcm07207)
static void sdhc_transfer_abort(struct mmc_host *host)
{
	u32 proctl = read32(host->base + SDHC(PROCTL));
	u32 val;

	val = read32(host->base + SDHC(SYSCTL));
	val |= 0x3000000;
	write32(host->base + SDHC(SYSCTL), val);
	while ((read32(host->base + SDHC(SYSCTL)) & 0x3000000));

	write32(host->base + SDHC(PROCTL), proctl);
	write32(host->base + SDHC(WML), 0x00800080);

	sdhc_set_clock(host, 1);
}
#endif

static int sdhc_pio_transfer_data(struct mmc_host *host,
				   struct mmc_command *cmd)
{
	u16 *buffer = (u16 *)cmd->data->buf;
	int i;

	trace();

	while(!(read16(host->base + SDHI_INFO2) & 0x0100));
	if (!(read16(host->base + SDHI_INFO2) & 0x0100))
		dev_err("DATA not Ready\n");

	for (i=0; i<cmd->data->blocks; i++) {
		int blksz = (int)cmd->data->blksz;
		u16 stat;

		do {
			stat = read16(host->base + SDHI_INFO2);
			if (stat & 0x100)
				break;
			if (stat & 0x7f)
				dev_err("status = %p\n", stat);
		} while(1);

		write16(host->base + SDHI_INFO2, 0xfeff);

		while (blksz > 0) {
			*(buffer++) = read16(host->base + SDHI_BUF);
			blksz -= 2;
		}
	}

	return 0;
}

static int sdhc_do_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
	u16 val;
	u32 mode;
	int i;
	u32 stat, stat2;
	static int acmd = 0;

	trace();

	if (cmd->opcode == MMC_GO_IDLE_STATE) {
		val = read16(host->base + SDHI_CLK_CTRL);
		val |= 0x0100;
		val &= ~0x0200;
		write16(host->base + SDHI_CLK_CTRL, val);
		mdelay(10);
		write16(host->base + SDHI_CLK_CTRL, val);
	}

	sdhc_clock_start(host);

	/* Waiting for SD Bus busy to be cleared */
	while (!(read16(host->base + SDHI_INFO2) & 0x2000));

	if (cmd->debug)
		dev_dbg("CMD%d\n", cmd->opcode);

	mode = cmd->opcode;
	if (acmd)
		mode |= (1 << 6);

	if (cmd->opcode == 55)
		acmd = 1;
	else
		acmd = 0;

	if (cmd->data) {
		mode |= (1 << 11);
		if (cmd->data->flags & MMC_DATA_READ)
			mode |= (1 << 12);

		if (cmd->data->blocks > 1) {
			mode |= (1 << 13);
			write16(host->base + SDHI_STOP, 0x100);

			write16(host->base + SDHI_SECCNT, cmd->data->blocks);
			write16(host->base + SDHI_SIZE, 512);
		} else {
			write16(host->base + SDHI_SIZE, cmd->data->blksz);
		}
	}

	/* status clear */
	write16(host->base + SDHI_INFO1, 0x0000);
	write16(host->base + SDHI_INFO2, 0x0000);

	write16(host->base + SDHI_ARG0, cmd->arg & 0xffff);
	write16(host->base + SDHI_ARG1, (cmd->arg >> 16) & 0xffff);
	if (cmd->debug)
		dev_dbg("XFERTYP: %p\n", mode);

	write16(host->base + SDHI_CMD, mode);


	i=1000;
	if (cmd->opcode != 12) {
		for (i=1000000; i; i--) {
			stat = read16(host->base + SDHI_INFO1);
			if (stat & 0x1)
				break;
			udelay(1);
		}
	}
	write16(host->base + SDHI_INFO1, 0xfffe);

	stat2 = read16(host->base + SDHI_INFO2);
	if ((i <= 0) || (stat2 & 0x7f)) {
		if (cmd->opcode != SD_SEND_IF_COND)
			dev_err("CMD:%d timedout. (status = %p)\n",
				cmd->opcode, stat2, i);
		return -H_EIO;
	}

	if (cmd->flags & MMC_RSP_136) {
		u32 rsp[8];

		for (i=0; i<8; i++)
			rsp[i] = (u32)read16(host->base + SDHI_RSP0 + 2 * i);

		cmd->resp[3] = (((rsp[1] << 24) & 0xff000000) |
				((rsp[0] << 8) & 0xff00));
		cmd->resp[2] = (((rsp[3] << 24) & 0xff000000) |
				(rsp[2] << 8) | ((rsp[1] >> 8) & 0x00ff));
		cmd->resp[1] = (((rsp[5] << 24) & 0xff000000) |
				(rsp[4] << 8) | ((rsp[3] >> 8) & 0x00ff));
		cmd->resp[0] = (((rsp[7] << 24) & 0xff000000) |
				(rsp[6] << 8) | ((rsp[5] >> 8) & 0x00ff));
	} else {
		cmd->resp[0] = ((read16(host->base + SDHI_RSP1) << 16) |
				read16(host->base + SDHI_RSP0));
	}

	if (cmd->debug)
		dev_dbg("RESP: %p, %p, %p, %p\n",
			cmd->resp[0], cmd->resp[1],
			cmd->resp[2], cmd->resp[3]);

	if (cmd->data) {
		sdhc_pio_transfer_data(host, cmd);

		if (cmd->data->stop) {
			int ret;
#if defined(CONFIG_MMCSD_RMA1_SDHC_ERRATA_ENGcm07207)
			/* errata: ENGcm07207 */
			sdhc_transfer_abort(host);
#endif
			ret = sdhc_do_cmd(host, cmd->data->stop);
			if (ret)
				return ret;
		}
	}

	return 0;
}

static int sdhc_get_card_detect(struct mmc_host *host)
{
#if defined(SDHI0_ENABLE_EXTERNAL_CD)
	return !gpio_get_value(xSDSLOT1_CD);
#else
	u16 val;
	val = read16(host->base + SDHI_INFO1);
	if (val & 0x20)
		return 1; /* Card Detected */
#endif

	return 0;
}

static int sdhc_set_bus_width(struct mmc_host *host, u32 width)
{
	u32 val;

	trace();

	val = read16(host->base + SDHI_OPTION);

	switch (width) {
	case 4:
		val &= ~(1 << 15);
		break;
	default:
		val |= (1 << 15);
		break;
	}
	write16(host->base + SDHI_OPTION, val);

	return 0;
}

static void sdhc_reset(struct mmc_host *host)
{
	trace();

	mdelay(1);
}

static int sdhc_init(struct mmc_host *host)
{
	trace();

	rma1_sdhc_request_port(0);

	/* Regulator 3.3V ON, Card Power OFF */
	gpio_direction_output(xSDHI0_VS_B, 0);
	gpio_direction_output(xSDHI0_PON, 1);
	gpio_direction_output(xSDSLOT1_PON, 0);

	cpg_clr_mstpcr(314);


	sdhc_reset(host);

	mdelay(200);
	gpio_direction_output(xSDSLOT1_PON, 1); /* CARD Power ON */
	mdelay(10);

	write16(host->base + SDHI_SOFT_RST, 0x1);
	sdhc_set_clock(host, 0);
	sdhc_set_bus_width(host, 1);

	return 0;
}

static void sdhc_exit(struct mmc_host *host)
{
	trace();
}

static struct mmc_host_ops rma1_sdhc_ops = {
	.get_cd = sdhc_get_card_detect,
	.do_cmd = sdhc_do_cmd,
	.set_clock = sdhc_set_clock,
	.set_bus_width = sdhc_set_bus_width,
};

struct mmc_host rma1_sdhc = {
	.init = sdhc_init,
	.exit = sdhc_exit,
	.ops = &rma1_sdhc_ops,
};
