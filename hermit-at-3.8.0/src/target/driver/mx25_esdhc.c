#define CORE_NAME "mx25_esdhc"

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
#include <arch/clock.h>
#include <mmcsd.h>

extern struct iomux_info mmcsd1_pins[];
extern struct iomux_info mmcsd1_cd_pin[];
extern struct iomux_info mmcsd1_pwren_pin[];

#if defined(CONFIG_MMCSD_MX25_ESDHC2)
extern struct iomux_info mmcsd2_pins[];
extern struct iomux_info mmcsd2_cd_pin[];
extern struct iomux_info mmcsd2_pwren_pin[];
#endif

static void esdhc_clock_start(struct mmc_host *host)
{
	u32 val;

	val = read32(host->base + ESDHC(SYSCTL));
	val |= 0x3;
	write32(host->base + ESDHC(SYSCTL), val);
}

static void esdhc_clock_stop(struct mmc_host *host)
{
	u32 val;

	val = read32(host->base + ESDHC(SYSCTL));
	val &= ~0xf;
	write32(host->base + ESDHC(SYSCTL), val);
}

static int esdhc_set_clock(struct mmc_host *host, u32 clock)
{
	u32 val;

	trace();

	val = read32(host->base + ESDHC(SYSCTL));
	val &= ~0xfff0;
	val |= 0xe0000;
	if (clock) {
		/* Max speed: 40MHz */
		val |= 0x0020;
	} else {
		/* Min speed: 375KHz */
		val |= 0x1090;
	}
	write32(host->base + ESDHC(SYSCTL), val);

	return 0;
}

#if defined(CONFIG_MMCSD_MX25_ESDHC_ERRATA_ENGcm07207)
static void esdhc_transfer_abort(struct mmc_host *host)
{
	u32 proctl = read32(host->base + ESDHC(PROCTL));
	u32 val;

	val = read32(host->base + ESDHC(SYSCTL));
	val |= 0x3000000;
	write32(host->base + ESDHC(SYSCTL), val);
	while ((read32(host->base + ESDHC(SYSCTL)) & 0x3000000));

	write32(host->base + ESDHC(PROCTL), proctl);
	write32(host->base + ESDHC(WML), 0x00800080);

	esdhc_set_clock(host, 1);
}
#endif

static int esdhc_pio_transfer_data(struct mmc_host *host,
				   struct mmc_command *cmd)
{
	u32 *buffer = (u32 *)cmd->data->buf;
	int i;

	for (i=0; i<cmd->data->blocks; i++) {
		u32 blksz = cmd->data->blksz;

		while (!(read32(host->base + ESDHC(IRQSTAT)) & 0x20));

		while (blksz) {
			*(buffer++) = read32(host->base + ESDHC(DATPORT));
			blksz -= 4;
		}

		write32(host->base + ESDHC(IRQSTAT), 0x20);
	}

	return 0;
}

static int esdhc_do_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
	u32 val;
	u32 mode;
	int i;
	u32 stat;

	esdhc_clock_start(host);

	val = read32(host->base + ESDHC(SYSCTL));
	if (cmd->opcode == MMC_GO_IDLE_STATE)
		val |= 0x08000000;
	else
		val &= ~0x08000000;
	write32(host->base + ESDHC(SYSCTL), val);

	/* waiting for initialization */
	do {
		val = read32(host->base + ESDHC(SYSCTL));
	} while (val & 0x08000000);

	if (cmd->debug)
		dev_dbg("CMD%d, SYSCTL: %p\n", cmd->opcode, val);

	mode = cmd->opcode << 24;
	if (!(cmd->flags & MMC_RSP_PRESENT))
		mode |= 0x0 << 16;
	else if (cmd->flags & MMC_RSP_136)
		mode |= 0x1 << 16;
	else if (cmd->flags & MMC_RSP_BUSY)
		mode |= 0x3 << 16;
	else
		mode |= 0x2 << 16;

	if (cmd->flags & MMC_RSP_CRC)
		mode |= 0x8 << 16;
	if (cmd->flags & MMC_RSP_OPCODE)
		mode |= 0x10 << 16;
	if (cmd->data) {
		mode |= 0x20 <<16;
		mode |= 0x2;
		if (cmd->data->flags & MMC_DATA_READ)
			mode |= 0x10;

		if (cmd->data->blocks > 1)
			mode |= 0x20;

		write32(host->base + ESDHC(BLKATTR),
			cmd->data->blksz | (cmd->data->blocks << 16));
	}

	write32(host->base + ESDHC(IRQSTAT), 0xffffffff);
	write32(host->base + ESDHC(CMDARG), cmd->arg);
	if (cmd->debug)
		dev_dbg("XFERTYP: %p\n", mode);

	write32(host->base + ESDHC(XFERTYP), mode);

	for (i=1000000; i; i--) {
		stat = read32(host->base + ESDHC(IRQSTAT));
		if (stat & 0x1)
			break;
		udelay(1);
	}

	if ((i <= 0) || (stat & 0xffff0000)) {
		if (cmd->opcode != SD_SEND_IF_COND)
			dev_dbg("CMD:%d timedout. (status = %p)\n",
				cmd->opcode, stat, i);
		return -H_EIO;
	}

	if (cmd->flags & MMC_RSP_136) {
		u32 rsp[4];
		rsp[0] = read32(host->base + ESDHC(CMDRSP0));
		rsp[1] = read32(host->base + ESDHC(CMDRSP1));
		rsp[2] = read32(host->base + ESDHC(CMDRSP2));
		rsp[3] = read32(host->base + ESDHC(CMDRSP3));

		cmd->resp[0] = rsp[3] << 8;
		cmd->resp[0] |= (rsp[2] >> 24) & 0xff;
		cmd->resp[1] = rsp[2] << 8;
		cmd->resp[1] |= (rsp[1] >> 24) & 0xff;
		cmd->resp[2] = rsp[1] << 8;
		cmd->resp[2] |= (rsp[0] >> 24) & 0xff;
		cmd->resp[3] = rsp[0] << 8;
	} else {
		cmd->resp[0] = read32(host->base + ESDHC(CMDRSP0));
	}

	if (cmd->debug)
		dev_dbg("RESP: %p, %p, %p, %p\n",
			cmd->resp[0], cmd->resp[1],
			cmd->resp[2], cmd->resp[3]);

	if (cmd->data) {
		esdhc_pio_transfer_data(host, cmd);

		if (cmd->data->stop) {
			int ret;
#if defined(CONFIG_MMCSD_MX25_ESDHC_ERRATA_ENGcm07207)
			/* errata: ENGcm07207 */
			esdhc_transfer_abort(host);
#endif
			ret = esdhc_do_cmd(host, cmd->data->stop);
			if (ret)
				return ret;
		}
	}

	esdhc_clock_stop(host);

	return 0;
}

static int esdhc_get_card_detect(struct mmc_host *host)
{
	int pin;

	trace();

	if (host->base == ESDHC1_BASE_ADDR)
		pin = mmcsd1_cd_pin[0].pin;
#if defined(CONFIG_MMCSD_MX25_ESDHC2)
	else if (host->base == ESDHC2_BASE_ADDR)
		pin = mmcsd2_cd_pin[0].pin;
#endif
	else
		return -H_EINVAL;

	return !mx25_gpio_get_psr(pin);
}

static int esdhc_set_bus_width(struct mmc_host *host, u32 width)
{
	u32 val;

	trace();

	val = read32(host->base + ESDHC(PROCTL));
	val &= ~0x6;

	switch (width) {
	case 4:
		val |= 0x2;
		break;
	default:
		break;
	}
	write32(host->base + ESDHC(PROCTL), val);

	return 0;
}

static void esdhc_reset(struct mmc_host *host)
{
	u32 val;

	trace();

	val = read32(host->base + ESDHC(SYSCTL));
	val |= 0x07000000;
	write32(host->base + ESDHC(SYSCTL), val);

	mdelay(1);
}

static int esdhc_init(struct mmc_host *host)
{
	int pin;
	int assert;

	trace();

	if (host->base == ESDHC1_BASE_ADDR) {
		pin = mmcsd1_pwren_pin[0].pin;
		assert = 0;
	}
#if defined(CONFIG_MMCSD_MX25_ESDHC2)
	else if (host->base == ESDHC2_BASE_ADDR) {
		pin = mmcsd2_pwren_pin[0].pin;
		assert = 1;
	}
#endif
	else
		return -H_EINVAL;

	esdhc_reset(host);
	mx25_gpio_output(pin, assert ? 0 : 1);
	mdelay(200);
	mx25_gpio_output(pin, assert ? 1 : 0);
	mdelay(10);

	write32(host->base + ESDHC(WML), 0x00800080);

	return 0;
}

static void esdhc_exit(struct mmc_host *host)
{
	trace();

	if (host->base == ESDHC1_BASE_ADDR) {
		mx25_iomux_config(mmcsd1_pins, 0);
		mx25_gpio_output(mmcsd1_pins[1].pin, 0); /* SDCLK: Low */
		mx25_gpio_output(mmcsd1_pwren_pin[0].pin, 1);
	}
#if defined(CONFIG_MMCSD_MX25_ESDHC2)
	else if (host->base == ESDHC2_BASE_ADDR) {
		mx25_iomux_config(mmcsd2_pins, 0);
		mx25_gpio_output(mmcsd2_pins[1].pin, 0); /* SDCLK: Low */
		mx25_gpio_output(mmcsd2_pwren_pin[0].pin, 1);
	}
#endif
}

static struct mmc_host_ops mx25_sdhc_ops = {
	.get_cd = esdhc_get_card_detect,
	.do_cmd = esdhc_do_cmd,
	.set_clock = esdhc_set_clock,
	.set_bus_width = esdhc_set_bus_width,
};

static struct mmc_host mx25_sdhc = {
	.init = esdhc_init,
	.exit = esdhc_exit,
	.ops = &mx25_sdhc_ops,
};

static int mx25_mmcsd_probe(struct mmcsd_device *dev, int port)
{
	trace();

	switch (port) {
	case 0:
		mx25_iomux_config(mmcsd1_pins, 1);
		mx25_iomux_config(mmcsd1_cd_pin, 1);
		mx25_iomux_config(mmcsd1_pwren_pin, 1);
		mx25_ahb_clock_enable(5);
		mx25_per_clock_enable(3);
		mx25_ipg_clock_enable(13);
		dev->host = &mx25_sdhc;
		dev->host->base = ESDHC1_BASE_ADDR;
		break;
#if defined(CONFIG_MMCSD_MX25_ESDHC2)
	case 1:
		mx25_iomux_config(mmcsd2_pins, 1);
		mx25_iomux_config(mmcsd2_cd_pin, 1);
		mx25_iomux_config(mmcsd2_pwren_pin, 1);
		mx25_ahb_clock_enable(6);
		mx25_per_clock_enable(4);
		mx25_ipg_clock_enable(14);
		dev->host = &mx25_sdhc;
		dev->host->base = ESDHC2_BASE_ADDR;
		break;
#endif
	default:
		hprintf("error\n");
		return -H_EINVAL;
	}

	return 0;
}

static void mx25_mmcsd_remove(struct mmcsd_device *dev)
{
	trace();
}

struct mmcsd_device mx25_mmcsd = {
	.probe = mx25_mmcsd_probe,
	.remove = mx25_mmcsd_remove,
};
