#define CORE_NAME "eth_rma1"

#include <autoconf.h>
#if defined(CONFIG_ETHERNET_DEBUG)
#define DEBUG
#define VERBOSE_DEBUG
#define TRACE_DEBUG
#endif
#include <hermit.h>
#include <herrno.h>
#include <io.h>
#include <arch/pinmux.h>
#include <string.h>
#include <mac.h>
#include <net/net.h>
#include <net/eth.h>
#include <net/eth_util.h>
#include <delay.h>
#include <rma1_gpio.h>
#include <platform/pinmux.h>

extern int rma1_eth_request_port(void);

#define GETHER_EDSR		0xe9a00000
#define GETHER_TDLAR		0xe9a00010
#define GETHER_TDFAR		0xe9a00014
#define GETHER_TDFXR		0xe9a00018
#define GETHER_TDFFR		0xe9a0001c
#define GETHER_RDLAR		0xe9a00030
#define GETHER_RDFAR		0xe9a00034
#define GETHER_RDFXR		0xe9a00038
#define GETHER_RDFFR		0xe9a0003c
#define GETHER_EDMR		0xe9a00400
#define GETHER_EDTRR		0xe9a00408
#define GETHER_EDRRR		0xe9a00410
#define GETHER_EESR		0xe9a00428
#define GETHER_EESIPR		0xe9a00430
#define GETHER_TRSCER		0xe9a00438
#define GETHER_TFTR		0xe9a00448
#define GETHER_FDR		0xe9a00450
#define GETHER_RMCR		0xe9a00458
#define GETHER_RPADIR		0xe9a00460
#define GETHER_FCFTR		0xe9a00468
#define GETHER_ECMR		0xe9a00500
#define GETHER_RFLR		0xe9a00508
#define GETHER_ECSR		0xe9a00510
#define GETHER_ECSIPR		0xe9a00518
#define GETHER_PIR0		0xe9a00520
#	define	PIR_MDC	(1 << 0)
#	define	PIR_MMD (1 << 1)
#	define	PIR_MDO (1 << 2)
#	define	PIR_MDI	(1 << 3)
#define GETHER_PIPR		0xe9a0052c
#define GETHER_APR		0xe9a00554
#define GETHER_MPR		0xe9a00558
#define GETHER_TPAUSER		0xe9a00564
#define GETHER_GECMR		0xe9a005b0
#define GETHER_BCULR		0xe9a005b4
#define GETHER_MAHR		0xe9a005c0
#define GETHER_MALR		0xe9a005c8
#define GETHER_RMII_MII		0xe9a00790

#define GETHER_ARSTR		0xe9a01800
#define GETHER_TSU_FWEN0	0xe9a01810
#define GETHER_TSU_FWEN1	0xe9a01814
#define GETHER_TSU_FCM		0xe9a01818
#define GETHER_TSU_BSYSL0	0xe9a01820
#define GETHER_TSU_BSYSL1	0xe9a01824
#define GETHER_TSU_PRISL0	0xe9a01828
#define GETHER_TSU_PRISL1	0xe9a0182c
#define GETHER_TSU_FWSL0	0xe9a01830
#define GETHER_TSU_FWSL1	0xe9a01834
#define GETHER_TSU_FWSLC	0xe9a01838
#define GETHER_TSU_QTAG0	0xe9a01840
#define GETHER_TSU_QTAG1	0xe9a01844
#define GETHER_TSU_FWSR		0xe9a01850
#define GETHER_TSU_FWINMK	0xe9a01854
#define GETHER_TSU_TEN		0xe9a01864
#define GETHER_TSU_POST1	0xe9a01870
#define GETHER_TSU_POST2	0xe9a01874
#define GETHER_TSU_POST3	0xe9a01878
#define GETHER_TSU_POST4	0xe9a0187c

struct tx_desc {
	union {
		u32 v;
		struct {
			u32 tfs:12,
			    rsv:14,
			    twbi:1,
			    tfe:1,
			    tfp:2,
			    tdle:1,
			    tact:1;
		} s;
	} td0;
	union {
		u32 v;
		struct {
			u32 rsv:16,
			    tdl:16;
		} s;
	} td1;
	union {
		u32 v;
		struct {
			u32 tba;
		} s;
	} td2;
	u32 padding;
};

struct rx_desc {
	union {
		u32 v;
		struct {
			u32 rcs:16,
			    rfs:10,
			    rcse:1,
			    rfe:1,
			    rfp:2,
			    rdle:1,
			    ract:1;
		} s;
	} rd0;
	union {
		u32 v;
		struct {
			u32 rdl:16,
			    rbl:16;
		} s;
	} rd1;
	union {
		u32 v;
		struct {
			u32 rba;
		} s;
	} rd2;
	u32 padding;
};

#define nr_tx_desc (4)
#define nr_rx_desc (8)
#define desc_buf_size (2048)
static volatile struct tx_desc *tx_desc = (struct tx_desc *)0x40b00000;
static volatile struct rx_desc *rx_desc = (struct rx_desc *)0x40b20000;
static volatile u8 *tx_buf = (u8 *)0x40b40000;
static volatile u8 *rx_buf = (u8 *)0x40b80000;
static volatile struct tx_desc *tx_desc_cur = (struct tx_desc *)0x40b00000;
static volatile struct rx_desc *rx_desc_cur = (struct rx_desc *)0x40b20000;

static void dump(u32 addr, int len)
{
#if defined(DEBUG_DUMP)
	u8 *ptr = (u8 *)addr;
	int i;

	for (i=0; i<len; i++) {
		dev_dbg_r("%b ", *ptr++);
		if (i%16==15)
			dev_dbg_r("\n");
	}
	if (i%16!=16)
		dev_dbg_r("\n");
#endif
}

static u8 __mii_read_bit(void)
{
	u32 pir;
	write32(GETHER_PIR0, PIR_MDC);
	udelay(1);
	write32(GETHER_PIR0, 0);
	udelay(2);
	pir = read32(GETHER_PIR0);
	write32(GETHER_PIR0, PIR_MDC);
	udelay(1);

	return pir & PIR_MDI;
}

static void __mii_write_bit(u32 level)
{
	u32 mdo = level ? PIR_MDO : 0;

	write32(GETHER_PIR0, mdo | PIR_MMD | PIR_MDC);
	udelay(1);
	write32(GETHER_PIR0, mdo | PIR_MMD);
	udelay(2);
	write32(GETHER_PIR0, mdo | PIR_MMD | PIR_MDC);
	udelay(1);
}

static u16 mdio_read(struct net_device *dev, addr_t addr, addr_t reg, u16 *val)
{
	u16 _val = 0;
	int i;

	/* Preamble */
	for (i=0; i<32; i++)
		__mii_write_bit(1);

	/* Start of Frame */
	__mii_write_bit(0);
	__mii_write_bit(1);

	/* OP Code: Read */
	__mii_write_bit(1);
	__mii_write_bit(0);

	/* PHY Address */
	for (i=4; i>=0; i--)
		__mii_write_bit(addr & (1 << i));

	/* Register Address */
	for (i=4; i>=0; i--)
		__mii_write_bit(reg & (1 << i));

	/* Turn Around */
	__mii_read_bit();
	__mii_read_bit();

	/* Data */
	for (i=15; i>=0; i--)
		_val |= ((__mii_read_bit() ? 1 : 0) << i);

	if (val)
		*val = _val;

	return 0;
}

static int mdio_write(struct net_device *dev, addr_t addr, addr_t reg, u16 val)
{
	int i;

	/* Preamble */
	for (i=0; i<32; i++)
		__mii_write_bit(1);

	/* Start of Frame */
	__mii_write_bit(0);
	__mii_write_bit(1);

	/* OP Code: Write */
	__mii_write_bit(0);
	__mii_write_bit(1);

	/* PHY Address */
	for (i=4; i>=0; i--)
		__mii_write_bit(addr & (1 << i));

	/* Register Address */
	for (i=4; i>=0; i--)
		__mii_write_bit(reg & (1 << i));

	/* Turn Around */
	__mii_write_bit(0);
	__mii_write_bit(0);

	/* Data */
	for (i=15; i>=0; i--)
		__mii_write_bit(val & (1 << i));

	return 0;
}

static void rma1_enable_phy(struct net_device *dev)
{
	trace();
}

static void rma1_disable_phy(struct net_device *dev)
{
	trace();
}

static void rma1_phy_reset(struct net_device *dev)
{
	u16 val;

	/* Hardware reset using nRST for LAN8710 */
	gpio_set_value(xETH_PHYRST, 0);
	udelay(100);
	gpio_set_value(xETH_PHYRST, 1);
	udelay(1);

	/* Software reset */
	mdio_write(dev, 0x00, 0x00, 0x8000);
	do {
		mdio_read(dev, 0, 0, &val);
	} while (val & 0x8000);
}

static int rma1_phy_init(struct net_device *dev)
{
	int timeout;
	u16 val;
	u32 id;
	u16 id1, id2;

	trace();

	rma1_phy_reset(dev);

	mdio_read(dev, 0, 2, &id1);
	mdio_read(dev, 0, 3, &id2);
	id = (id1 << 16 | id2);
	dev_dbg("PHYID = %p, REV = %d\n", id >> 4, id & 0x0f);

	/* starting auto-negotiation and waiting for completion */
	mdio_write(dev, 0x00, 0x00, 0x1200);
	timeout = 5000;
	while (--timeout) {
		mdio_read(dev, 0x00, 0x01, &val);
		if (val & 0x0020)
			break;
		mdelay(1);
		dev_vdbg("Auto-negotiation Satus: %p\n", val);
	}
	if (timeout <= 0) {
		dev_dbg("failed to Auto-negotiation: %p\n", val);
		return -H_EIO;
	}
	dev_dbg("Auto-negotiation Completed: %p\n", val);

	/* waiting for link-up */
	mdelay(300);

	mdio_read(dev, 0x00, 0x01, &val);
	if (!(val & 0x04)) {
		dev_dbg_r("PHY: Link is down!\n");
		return -H_EIO;
	}

#if defined(DEBUG)
	{
		u16 adv, lpa;
		char *speed, *duplex;

		mdio_read(dev, 0x00, 4, &adv);
		mdio_read(dev, 0x00, 5, &lpa);
		lpa &= adv;
		if (lpa & 0x0100) {
			speed = "100BASE-TX";
			duplex = "Full";
		} else if (lpa & 0x0080) {
			speed = "100BASE-TX";
			duplex = "Half";
		} else if (lpa & 0x0040) {
			speed = "10BASE-T";
			duplex = "Full";
		} else if (lpa & 0x0020) {
			speed = "10BASE-T";
			duplex = "Half";
		} else {
			speed = "unknown";
			duplex = "unknown";
		}
		dev_dbg_r("PHY: Link is up! %s/%s\n", speed, duplex);
	}
#endif

	return 0;
}

static void rma1_module_reset(void)
{
	trace();

	write32(GETHER_ARSTR, 1);
	mdelay(1);
	write32(GETHER_RMII_MII, 1); /* MII */
}

static void rma1_mac_reset(void)
{
	trace();

	write32(GETHER_EDSR, 3);
	write32(GETHER_EDMR, 0x03);
	while (read32(GETHER_EDMR) & 0x3);

	/* Table Init */
	write32(GETHER_TDLAR, 0);
	write32(GETHER_TDFAR, 0);
	write32(GETHER_TDFXR, 0);
	write32(GETHER_TDFFR, 0);
	write32(GETHER_RDLAR, 0);
	write32(GETHER_RDFAR, 0);
	write32(GETHER_RDFXR, 0);
	write32(GETHER_RDFFR, 0);
}

static void rma1_setup_tx_desc(struct net_device *dev)
{
	int i;

	trace();

	for(i=0; i<nr_tx_desc; i++) {
		memset((void *)&tx_desc[i], 0, sizeof(struct tx_desc));
		tx_desc[i].td0.v = 0;
		tx_desc[i].td0.s.tact = 0;
		tx_desc[i].td0.s.tfp = 3;
		tx_desc[i].td1.v = 0;
		tx_desc[i].td2.s.tba = (u32)(tx_buf + (desc_buf_size * i));
	}
	tx_desc[nr_tx_desc - 1].td0.s.tdle = 1;

	write32(GETHER_TDLAR, (u32)tx_desc);
	write32(GETHER_TDFAR, (u32)tx_desc);
	write32(GETHER_TDFXR, (u32)&tx_desc[nr_tx_desc - 1]);
	write32(GETHER_TDFFR, 1);
	tx_desc_cur = tx_desc;
}

static void rma1_setup_rx_desc(struct net_device *dev)
{
	int i;

	trace();

	for (i=0; i<nr_rx_desc; i++) {
		memset((void *)&rx_desc[i], 0, sizeof(struct rx_desc));
		rx_desc[i].rd0.s.ract = 1;
		rx_desc[i].rd1.s.rbl = desc_buf_size;
		rx_desc[i].rd2.s.rba = (u32)(rx_buf + (desc_buf_size * i));
	}
	rx_desc[nr_rx_desc - 1].rd0.s.rdle = 1;

	write32(GETHER_RDLAR, (u32)rx_desc);
	write32(GETHER_RDFAR, (u32)rx_desc);
	write32(GETHER_RDFXR, (u32)&rx_desc[nr_rx_desc - 1]);
	write32(GETHER_RDFFR, 1);
	rx_desc_cur = rx_desc;
}

static int rma1_send(struct net_device *dev, struct net_packet *pkt)
{
	volatile struct tx_desc *desc;
	int len;
	u8 *buf;
	int to = 100000; /* micro seconds */

	trace();

	if (tx_desc_cur == &tx_desc[nr_tx_desc])
		tx_desc_cur = tx_desc;
	desc = tx_desc_cur;

	buf = (u8 *)desc->td2.s.tba;
	memcpy(buf, pkt->eth, ETH_FRAME_LEN);
	len = ETH_FRAME_LEN;
	memcpy(buf + len, pkt->f2.any, pkt->f2len);
	len += pkt->f2len;
	memcpy(buf + len, pkt->f3.any, pkt->f3len);
	len += pkt->f3len;
	memcpy(buf + len, pkt->f4, pkt->f4len);
	len += pkt->f4len;

	if (len < 60)
		len = 60;

	/* clear status */
	write32(GETHER_EESR, 0xe0200000);

	desc->td0.s.tfs = 0;
	desc->td0.s.tfe = 0;
	desc->td0.s.tfp = 3;
	desc->td0.s.twbi = 1;
	desc->td1.s.tdl = len;
	desc->td0.s.tact = 1;

	while ((read32(GETHER_EDTRR) & 0x3) != 0x03)
		write32(GETHER_EDTRR, 0x3);

	while (--to) {
		if ((read32(GETHER_EESR) & 0xe0200000) == 0xe0200000) {
			write32(GETHER_EESR, 0xe0200000);
			break;
		}
		udelay(1);
	}
	if (to <= 0)
		return -H_ETIMEDOUT;

	if (desc->td0.s.tact || desc->td0.s.tfe) {
		dev_dbg("Tx error: TD0:%p, EESR=%p,EDTRR=%p\n",
			desc->td0.v, read32(GETHER_EESR), read32(GETHER_EDTRR));
		desc->td0.s.tact = 0;
		tx_desc_cur++;
		return -H_EAGAIN;
	}

	tx_desc_cur++;

	return 0;
}

static int rma1_recv(struct net_device *dev, struct net_packet *pkt,
		     int *timeout)
{
	volatile struct rx_desc *desc;
	int to = *timeout * 1000; /* micro seconds */

	trace();

	if (rx_desc_cur == &rx_desc[nr_rx_desc])
		rx_desc_cur = rx_desc;
	desc = rx_desc_cur;

	while (--to) {
		if (!desc->rd0.s.ract)
			break;
		udelay(1);
	}
	if (to <= 0)
		return -H_ETIMEDOUT;

	if (!desc->rd0.s.ract) {
		if (!(desc->rd0.s.rfp & 1) ||
		    (desc->rd0.s.rfe && (desc->rd0.s.rfs & 0x23f))) {
			dev_dbg("Rx error: RD0:%p\n", desc->rd0.v);
			desc->rd0.s.rcs = 0;
			desc->rd0.s.rfs = 0;
			desc->rd0.s.rcse = 0;
			desc->rd0.s.rfe = 0;
			desc->rd0.s.rfp = 0;
			desc->rd1.s.rdl = 0;
			desc->rd0.s.ract = 1;

			if ((read32(GETHER_EDRRR) & 1) == 0)
				write32(GETHER_EDRRR, 1);

			rx_desc_cur++;

			return -H_EAGAIN;
		}

		dump(desc->rd2.s.rba, desc->rd1.s.rdl);
		memcpy(pkt->raw_data_addr, (void *)desc->rd2.s.rba,
		       desc->rd1.s.rdl);
		pkt->raw_data_len = desc->rd1.s.rdl;

		desc->rd0.s.rcs = 0;
		desc->rd0.s.rfs = 0;
		desc->rd0.s.rcse = 0;
		desc->rd0.s.rfe = 0;
		desc->rd0.s.rfp = 0;
		desc->rd1.s.rdl = 0;
		desc->rd0.s.ract = 1;

		if ((read32(GETHER_EDRRR) & 1) == 0)
			write32(GETHER_EDRRR, 1);

		rx_desc_cur++;

		return 0;
	}
	dev_dbg("Not Received!\n");

	return -H_EIO;
}

static void rma1_set_mac(struct net_device *dev, unsigned char *mac)
{
	u32 mac_val;
	mac_val = (mac[0] << 24 |
		   mac[1] << 16 |
		   mac[2] << 8 |
		   mac[3] << 0);
	write32(GETHER_MAHR, mac_val);
	mac_val = (mac[4] << 8 |
		   mac[5] << 0);
	write32(GETHER_MALR, mac_val);
}

static int rma1_eth_init(struct net_device *dev)
{
	u32 gecmr, ecmr;
	u16 adv, lpa;
	int ret;

	trace();

	rma1_eth_request_port();
	gpio_direction_output(xETH_PHYRST, 0);

	if (!is_valid_mac(dev->eth_mac))
		get_mac_address(dev->eth_mac);

	/* GETHER module enable */
	cpg_clr_mstpcr(309);

	rma1_module_reset();
	rma1_mac_reset();

	ret = rma1_phy_init(dev);
	if (ret) {
		dev_info_r("Link is down\n");
		return ret;
	}

	/* setup rx_desc */
	rma1_setup_tx_desc(dev);
	rma1_setup_rx_desc(dev);

	/* setup dmac */
	write32(GETHER_EESIPR, 0);
	write32(GETHER_EDMR, 0x40);
	write32(GETHER_FDR, 0x00000707);
	write32(GETHER_TFTR, 0);
	write32(GETHER_RMCR, 0x00000000);
	write32(GETHER_TRSCER, 0);
	write32(GETHER_BCULR, 0x800);
	write32(GETHER_FCFTR, 0x00070007);
	write32(GETHER_RFLR, 0x1000);

	write32(GETHER_EESR, read32(GETHER_EESR));
	write32(GETHER_ECSIPR, 0x017fffff);

	/* setup emac */
	rma1_set_mac(dev, dev->eth_mac);

	write32(GETHER_PIPR, 0x00);
	write32(GETHER_APR, 0x1);
	write32(GETHER_MPR, 0x1);
	write32(GETHER_TPAUSER, 0);

	write32(GETHER_ECMR, 0x00080060);

	/* setup speed/duplex */
	gecmr = 0;
	ecmr = read32(GETHER_ECMR) & ~0x2;
	mdio_read(dev, 0x00, 4, &adv);
	mdio_read(dev, 0x00, 5, &lpa);
	lpa &= adv;
	if (lpa & 0x0100) {
		/* 100BASE-TX/Full */
		gecmr = 0x04;
		ecmr |= 0x02;
	} else if (lpa & 0x0080) {
		/* 100BASE-TX/Half */
		gecmr = 0x04;
	} else if (lpa & 0x0040) {
		/* 10BASE-T/Full */
		ecmr |= 0x02;
	} else {
		/* 10BASE-T/Half */
		;
	}

	write32(GETHER_GECMR, gecmr);
	write32(GETHER_ECMR, ecmr);

	write32(GETHER_ECSR, 0x3);
	write32(GETHER_ECMR, ecmr | 0x2000);
	write32(GETHER_EDRRR, 1);

	return 0;
}

static int rma1_eth_shutdown(struct net_device *dev)
{
	trace();

	return 0;
}

struct net_device rma1_eth = {
	.init = rma1_eth_init,
	.exit = rma1_eth_shutdown,

	.set_mac = rma1_set_mac,
	.enable_phy = rma1_enable_phy,
	.disable_phy = rma1_disable_phy,
	.send = rma1_send,
	.recv = rma1_recv,
};
