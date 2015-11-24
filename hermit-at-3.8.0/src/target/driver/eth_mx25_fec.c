#define CORE_NAME "fec"

#include <autoconf.h>
#if defined(CONFIG_ETHERNET_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <platform.h>
#include <herrno.h>
#include <io.h>
#include <string.h>
#include <delay.h>
#include <mac.h>
#include <net/net.h>
#include <net/eth.h>
#include <net/eth_util.h>
#include <arch/system.h>
#include <arch/iomux.h>
#include <arch/clock.h>

extern struct iomux_info fec_pins[];
extern struct iomux_info fec_phy_reset_pin[];
extern struct iomux_info fec_led_pin[];

static void mx25_fec_link_led(int mode)
{
	mx25_gpio_output(fec_led_pin[0].pin, !mode);
}

static void mx25_fec_phy_reset(int reset)
{
	trace();

	mx25_gpio_output(fec_phy_reset_pin[0].pin, !reset);
}

#if defined(VERBOSE_DEBUG)
static void dump(u8 *buf, size_t len)
{
	int i;
	for (i=0; i<len; i++) {
		dev_vdbg_r("%b ", buf[i]);
		if (i%16 == 15)
			dev_vdbg_r("\n");
	}
	if (i%16!=0)
		dev_vdbg_r("\n");
}
#else
#define dump(buf, len)
#endif

static int mdio_wait_xfer(struct net_device *dev)
{
	int timeout = 100;
	while (--timeout) {
		if (read32(dev->base_addr + FEC_EIR) & 0x800000) {
			write32(dev->base_addr + FEC_EIR, 0x800000);
			return 0;
		}
		udelay(1);
	}
	dev_dbg("%s(): timeout\n", __FUNCTION__);

	return -H_EIO;
}

static int mdio_read(struct net_device *dev, addr_t addr, addr_t reg, u16 *val)
{
	write32(dev->base_addr + FEC_MMFR,
		0x60020000 | (addr << 23) | (reg << 18));
	mdio_wait_xfer(dev);

	*val = read32(dev->base_addr + FEC_MMFR) & 0xffff;

	dev_dbg("mdio_rd: phy(%d) = 0x%p\n", reg, *val);

	return 0;
}

static int mdio_write(struct net_device *dev, addr_t addr, addr_t reg, u16 val)
{
	write32(dev->base_addr + FEC_MMFR,
		0x50020000 | (addr << 23) | (reg << 18) | val);
	mdio_wait_xfer(dev);

	dev_dbg("mdio_wr: phy(%d) = 0x%p\n", reg, val);

	return 0;
}

static int phy_init(struct net_device *dev)
{
	int timeout;
	u16 val;
	trace();

	/* soft-reset */
	mdio_write(dev, 0x00, 0x00, 0x8000);
	while (1) {
		mdio_read(dev, 0x00, 0x00, &val);
		if (!(val & 0x8000))
			break;
	}

#if defined(DEBUG)
	/* id (debug) */
	mdio_read(dev, 0x00, 0x02, &val);
	dev_dbg("Phy ID: %p, ", val);
	mdio_read(dev, 0x00, 0x03, &val);
	dev_dbg_r("%p\n", val);
#endif

	/* wait auto-negotiation complete */
	mdio_write(dev, 0x00, 0x00, 0x1200);
	timeout = 30;
	while (--timeout) {
		mdio_read(dev, 0x00, 0x01, &val);
		if (val & 0x0020)
			break;
		mdelay(100);
	}

	/* waiting for link-up */
	mdelay(300);

	mdio_read(dev, 0x00, 0x01, &val);
	if (!(val & 0x04)) {
		dev_info_r("Link is down!");
		return -H_EIO;
	}

	return 0;
}

struct fec_desc {
	u16 length;
	u16 status;
	u8 *data;
};

#define nr_tx_desc (16)
#define nr_rx_desc (128)
#define desc_buf_size (2048)

static volatile struct fec_desc *tx_desc = (struct fec_desc *)0x80b00000;
static volatile struct fec_desc *rx_desc = (struct fec_desc *)0x80b20000;
static u8 *tx_buf = (u8 *)0x80b40000;
static u8 *rx_buf = (u8 *)0x80b80000;
static volatile struct fec_desc *cur_tx_desc;
static volatile struct fec_desc *cur_rx_desc;

static void fec_enable_phy_module(struct net_device *dev) { do { } while (0); }
static void fec_disable_phy_module(struct net_device *dev) { do { } while (0); }

static int fec_send(struct net_device *dev, struct net_packet *pkt)
{
	volatile struct fec_desc *desc = cur_tx_desc;
	int timeout = 1000000;

	trace();

	desc->status &= ~0x9c00;
	memcpy(desc->data, pkt->eth, ETH_FRAME_LEN);
	desc->length = ETH_FRAME_LEN;
	memcpy(desc->data + desc->length, pkt->f2.any, pkt->f2len);
	desc->length += pkt->f2len;
	memcpy(desc->data + desc->length, pkt->f3.any, pkt->f3len);
	desc->length += pkt->f3len;
	memcpy(desc->data + desc->length, pkt->f4, pkt->f4len);
	desc->length += pkt->f4len;

	if (desc->length < 60)
		desc->length = 60;
	desc->status |= 0x9c00;
	desc->status &= ~0x03ff; /* status clear */

	dump(desc->data, desc->length);

	write32(dev->base_addr + FEC_TDAR, 0x01000000);

	dev_dbg("len: *0x%p = %p\n", &desc->length, desc->length);
	dev_dbg("sta: *0x%p = %p\n", &desc->status, desc->status);
	dev_dbg("dat: *0x%p = %p\n", &desc->data, desc->data);

	while (--timeout) {
		u32 val = read32(dev->base_addr + FEC_EIR);
		dev_dbg("EIR: %p, TDAR: %p, sta: %p\n",
			val, read32(dev->base_addr+0x14), desc->status);
		if ((val & 0x0c000000) == 0x0c000000) {
			break;
		}
		if (val & 0x20000000) {
			dev_err("BABT error\n");
			break;
		}
		udelay(1);
	}
	if (timeout == 0) {
		dev_err(" TX error!\n");
		return -1;
	}

	write32(dev->base_addr + FEC_EIR, 0x0c000000);

	if (desc == &tx_desc[nr_tx_desc - 1])
		cur_tx_desc = &tx_desc[0];
	else
		cur_tx_desc = ++desc;

	return 0;
}

static int fec_recv(struct net_device *dev, struct net_packet *pkt, int *timeout)
{
	volatile struct fec_desc *desc = cur_rx_desc;
	int timeout_usec = (*timeout) * 1000;

	trace();

	write32(dev->base_addr + FEC_RDAR, 0x01000000);

	while (--timeout_usec) {
#if defined(DEBUG)
		if (desc->status & 0x7fff)
			dev_dbg("rx-desc->status: %p\n", desc->status);
#endif
		if (!(desc->status & 0x8000))
			break;
		udelay(1);
	}
	*timeout = timeout_usec / 1000;
	if (!timeout_usec) {
		dev_vdbg(" RX timeout!\n");
		return -1;
	}

	memcpy(pkt->raw_data_addr, desc->data, desc->length);
	pkt->raw_data_len = desc->length;

	dump(desc->data, desc->length);

	/* free rx_desc */
	desc->status &= 0x2000;
	desc->status |= 0x8000;
	desc->length = 0;

	if (++desc == &rx_desc[nr_rx_desc])
		cur_rx_desc = &rx_desc[0];
	else
		cur_rx_desc = desc;

	return 0;
}

static void fec_init_desc(struct net_device *dev)
{
	int i;

	trace();

	for (i=0; i<nr_tx_desc; i++) {
		tx_desc[i].status = 0;
		tx_desc[i].length = 0;
		tx_desc[i].data = &tx_buf[i * desc_buf_size];
	}
	tx_desc[i-1].status |= 0x2000;

	for (i=0; i<nr_rx_desc; i++) {
		rx_desc[i].status = 0x8000;
		rx_desc[i].length = 0;
		rx_desc[i].data = &rx_buf[i * desc_buf_size];
	}
	rx_desc[i-1].status |= 0x2000;

	write32(dev->base_addr + FEC_RDSR, (u32)rx_desc);
	write32(dev->base_addr + FEC_TDSR, (u32)tx_desc);
	write32(dev->base_addr + FEC_MRBR, 1520);

	cur_rx_desc = &rx_desc[0];
	cur_tx_desc = &tx_desc[0];
}

static int fec_init(struct net_device *dev)
{
	int ret;
	u32 rcr, tcr, mgcfgr;
	u16 spcs;

	trace();

	mx25_fec_phy_reset(1);
	udelay(100);
	mx25_fec_phy_reset(0);

	/* MAC reset */
	write32(dev->base_addr + FEC_ECR, 0x01);
	while (read32(dev->base_addr + FEC_ECR) & 0x01);

	write32(dev->base_addr + MIIGSK_ENR, 0x00000000);
	while (read32(dev->base_addr + MIIGSK_ENR) & 0x04);

	write32(dev->base_addr + FEC_EIMR, 0x0f800000);
	write32(dev->base_addr + FEC_EIR, 0xffffffff);

	/* MDC setup */
	write32(dev->base_addr + FEC_MSCR, 0x1c);

	ret = phy_init(dev);
	if (ret)
		return ret;

	mx25_fec_link_led(1);

	fec_init_desc(dev);

	dev->send = fec_send;
	dev->recv = fec_recv;
	dev->enable_phy = fec_enable_phy_module;
	dev->disable_phy = fec_disable_phy_module;

	get_mac_address(dev->eth_mac);
	write32(dev->base_addr + FEC_PALR, (dev->eth_mac[0] << 24 |
					    dev->eth_mac[1] << 16 |
					    dev->eth_mac[2] << 8 |
					    dev->eth_mac[3]));
	write32(dev->base_addr + FEC_PAUR, (dev->eth_mac[4] << 24 |
					    dev->eth_mac[5] << 16));

	dev_dbg("MAC = %b:%b:%b:%b:%b:%b\n",
		dev->eth_mac[0], dev->eth_mac[1], dev->eth_mac[2],
		dev->eth_mac[3], dev->eth_mac[4], dev->eth_mac[5]);

	write32(dev->base_addr + FEC_IAUR, 0x00000000);
	write32(dev->base_addr + FEC_IALR, 0x00000000);
	write32(dev->base_addr + FEC_GAUR, 0x00400000);
	write32(dev->base_addr + FEC_GALR, 0x00000000);

	mdio_read(dev, 0x00, 0x1f, &spcs);
	rcr = (1518 << 16) | 0x0004;
	tcr = 0x0004;
	if (spcs & 0x0010)
		/* full-duplex */
		tcr |= 0x4;
	else
		rcr |= 0x2;
	write32(dev->base_addr + FEC_RCR, rcr);
	write32(dev->base_addr + FEC_TCR, tcr);

	mgcfgr = 0x0001;
	if (spcs & 0x0004)
		/* 10BASE */
		mgcfgr |= 0x40;
	write32(dev->base_addr + MIIGSK_CFGR, mgcfgr);
	write32(dev->base_addr + MIIGSK_ENR, 0x00000002);

	write32(dev->base_addr + FEC_ECR, 0x00000002);
	write32(dev->base_addr + FEC_RDAR, 0x01000000);

	return 0;
}

static int fec_exit(struct net_device *dev)
{
	mx25_fec_link_led(0);

	mx25_fec_phy_reset(1);
	udelay(100);
	mx25_fec_phy_reset(0);

	/* MAC reset */
	write32(FEC_BASE_ADDR + FEC_ECR, 0x01);

	return 0;
}

struct net_device mx25_fec = {
	.init = fec_init,
	.exit = fec_exit,
};
