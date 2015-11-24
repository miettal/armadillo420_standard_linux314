#define CORE_NAME "eth_ns9750"

#include <autoconf.h>
#if defined(CONFIG_ETHERNET_DEBUG)
#define DEBUG
#define TRACE_DEBUG
#endif
#include <hermit.h>
#include <io.h>
#include <arch/ioregs.h>
#include <string.h>
#include <mac.h>
#include <net/net.h>
#include <net/eth.h>
#include <net/eth_util.h>
#include <delay.h>

#define NS_ETH_FRAME_LEN	(0x600)
#define NS_ETH_PHY_ADDRESS	(0x0000)
#define NS_MII_POLE_TIMEOUT	(10000)

#define LEN_RxBuf (1518+2+16)
#define LEN_TxBuf (1518+2+16)

#define DESC_SIZE	(0x10) /* 32bit * 4 */
#define DESC_SRC	(0x00)
#define DESC_LEN	(0x04)
#define DESC_DST	(0x08)
#define DESC_STA	(0x0c)

#define RX_DESC_BASE 0x01700000
static unsigned char *txbuf = (unsigned char *)0x01000000;
static unsigned char *rxbuf = (unsigned char *)0x00800000;

static u32 ns_mii_poll_busy(void)
{
	u32 timeout = NS_MII_POLE_TIMEOUT;

	while((read32(NS9750_ETH_PHYS_BASE + NS_ETH_MIND) & NS_ETH_MIND_BUSY)
	      && timeout--)
		udelay(1);
	return timeout;
}

static u16 ns_mii_read(u16 reg)
{
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MADR,
		(NS_ETH_PHY_ADDRESS << 8) | reg);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MCMD, NS_ETH_MCMD_READ);

	if(!ns_mii_poll_busy())
		dev_err("MII still busy in read\n");

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MCMD, 0);

	return (u16)read32(NS9750_ETH_PHYS_BASE + NS_ETH_MRDD);
}

static void ns_mii_write(u16 reg, u16 data)
{
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MADR,
		(NS_ETH_PHY_ADDRESS << 8) | reg);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MWTD, data);

	if(!ns_mii_poll_busy())
		dev_err("MII still busy in write\n");
}

static void ns_link_print_changed(void)
{
	u16 status;
	u16 control;

	control = ns_mii_read(PHY_COMMON_CTRL);

	if((control & PHY_COMMON_CTRL_AUTO_NEG) == PHY_COMMON_CTRL_AUTO_NEG){
		/* PHY_COMMON_STAT_LNK_STAT is only set on autonegotiation */
		status = ns_mii_read(PHY_COMMON_STAT);

		if(status & PHY_COMMON_STAT_LNK_STAT){
				status = ns_mii_read(PHY_LXT971_STAT2);
				status &= (PHY_LXT971_STAT2_100BTX |
					   PHY_LXT971_STAT2_DUPLEX_MODE |
					   PHY_LXT971_STAT2_AUTO_NEG);

				/* mask out all uninteresting parts */
		}
	}

	/* print new link status */
	dev_dbg("link mode %d Mbps %s duplex %s\n",
		(status & PHY_LXT971_STAT2_100BTX) ? 100 : 10,
		(status & PHY_LXT971_STAT2_DUPLEX_MODE) ? "full" : "half",
		(status & PHY_LXT971_STAT2_AUTO_NEG) ? "(auto)" : "");

}

static void ns_link_update_egcr(void)
{
	u32 status;
	u32 egcr;
	u32 mac2;
	u32 ipgt;

	egcr = read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1);
	mac2 = read32(NS9750_ETH_PHYS_BASE + NS_ETH_MAC2) & ~NS_ETH_MAC2_FULLD;
	ipgt = read32(NS9750_ETH_PHYS_BASE + NS_ETH_IPGT) & ~NS_ETH_IPGT_MA;

	status = ns_mii_read(PHY_LXT971_STAT2);

	if(status & PHY_LXT971_STAT2_DUPLEX_MODE){
		mac2 |= NS_ETH_MAC2_FULLD;
		ipgt |= 0x15;	/* see [1] p. 339 */
	}else{
		ipgt |= 0x12;	/* see [1] p. 339 */
	}

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MAC2, mac2);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1, egcr);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_IPGT, ipgt);
}

static void ns_link_auto_negotiate(void)
{
	u16 status;

	trace();

	status = ns_mii_read(PHY_COMMON_STAT);
	if((status & (PHY_COMMON_STAT_AN_COMP | PHY_COMMON_STAT_LNK_STAT)) ==
	   (PHY_COMMON_STAT_AN_COMP | PHY_COMMON_STAT_LNK_STAT)){
		ns_link_print_changed();
		ns_link_update_egcr();
		return;
	}

	/* run auto-negotation */
	/* define what we are capable of */
	ns_mii_write(PHY_COMMON_AUTO_ADV,
		     PHY_COMMON_AUTO_ADV_100BTXFD |
		     PHY_COMMON_AUTO_ADV_100BTX |
		     PHY_COMMON_AUTO_ADV_10BTFD |
		     PHY_COMMON_AUTO_ADV_10BT |
		     PHY_COMMON_AUTO_ADV_802_3);
	/* start auto-negotiation */
	ns_mii_write(PHY_COMMON_CTRL,
		     PHY_COMMON_CTRL_AUTO_NEG |
		     PHY_COMMON_CTRL_RES_AUTO);

	/* wait for completion */
	while(1){
		status = ns_mii_read(PHY_COMMON_STAT);
		if((status &
		    (PHY_COMMON_STAT_AN_COMP | PHY_COMMON_STAT_LNK_STAT)) ==
		   (PHY_COMMON_STAT_AN_COMP | PHY_COMMON_STAT_LNK_STAT)){
			/* lucky we are, auto-negotiation succeeded */
		  ns_link_print_changed();
		  ns_link_update_egcr();
			return;
		}
	}
}

static int ns_link_init(void)
{
	u16 val;

	trace();

	ns_link_auto_negotiate();

	val = ns_mii_read(PHY_COMMON_STAT);

	if(val & PHY_COMMON_STAT_LNK_STAT){
		return 0;
	}
	return -1;
}

static int phy_init(void)
{
	return ns_link_init();
}

void ns9750_enable_phy_module(struct net_device *dev)
{
}

void ns9750_disable_phy_module(struct net_device *dev)
{
}

int ns9750_eth_send(struct net_device *dev, struct net_packet *pkt)
{
	addr_t tx_desc = NS9750_ETH_PHYS_BASE + NS_ETH_TXBD;
	u32 txlen;

	trace();

	memcpy(txbuf, pkt->eth, ETH_FRAME_LEN);
	txlen = ETH_FRAME_LEN;
	memcpy(txbuf + txlen, pkt->f2.any, pkt->f2len);
	txlen += pkt->f2len;
	memcpy(txbuf + txlen, pkt->f3.any, pkt->f3len);
	txlen += pkt->f3len;
	memcpy(txbuf + txlen, pkt->f4, pkt->f4len);
	txlen += pkt->f4len;

	if (txlen < 60){
		memset(txbuf + txlen, 0, 60 - txlen);
		txlen = 60;
	}

	if(txlen > LEN_TxBuf){
		return -1;
	}

	write32(tx_desc + DESC_SRC, (u32)txbuf);
	write32(tx_desc + DESC_LEN, txlen);
	write32(tx_desc + DESC_STA, 0x00000000);
	write32(tx_desc + DESC_STA, 0xf0000000);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_TXPTR, 0);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_TXRPTR, 0);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1) | NS_ETH_EGCR1_ETXDMA);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR2,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR2) & ~NS_ETH_EGCR2_TCLER);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR2,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR2) | NS_ETH_EGCR2_TCLER);

	while(!(read32(tx_desc + DESC_STA) & NS_ETH_ETSR_TXOK));

	return 0;
}

static int ns9750_eth_rxbuf_free(struct net_device *dev, const int idx)
{
	addr_t rx_desc = RX_DESC_BASE + DESC_SIZE * idx;
	u32 val;
	trace();
	dev_dbg("idx: %d\n", idx);

	val = read32(rx_desc + DESC_STA);
	write32(rx_desc + DESC_STA, val & 0xe0000000);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_RXFREE, 0x1 << idx);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EINTR, NS_ETH_EINTR_RXDONEA >> idx);
	return 0;
}

int ns9750_eth_recv(struct net_device *dev, struct net_packet *pkt, int *timeout)
{
	addr_t rx_desc;
	int idx;
	u32 status;

	trace();

	while(1){
		idx = -1;
		while(idx == -1){
			int i;
			if(--(*timeout) <= 0) {
				return -1;
			}

			for (i=0; i<1000; i++) {
				status = read32(NS9750_ETH_PHYS_BASE + NS_ETH_EINTR);

				if(status & NS_ETH_EINTR_RXDONEA){
					idx = 0;
					break;
				}else if(status & NS_ETH_EINTR_RXDONEB){
					idx = 1;
					break;
				}else if(status & NS_ETH_EINTR_RXDONEC){
					idx = 2;
					break;
				}else if(status & NS_ETH_EINTR_RXDONED){
					idx = 3;
					break;
				}
				udelay(1);
			}
		}
		rx_desc = RX_DESC_BASE + DESC_SIZE * idx;

		pkt->raw_data_len = read32(rx_desc + DESC_LEN) - 4;
		memcpy(pkt->raw_data_addr, (void *)read32(rx_desc + DESC_SRC),
		       pkt->raw_data_len);
		pkt->eth = pkt->raw_data_addr;

		ns9750_eth_rxbuf_free(dev, idx);

		if(memcmp(pkt->eth->dmac, broadcast_mac, 6) == 0){
			dev_vdbg("broadcast packet detect\n");
		}else if(memcmp(pkt->eth->dmac, dev->eth_mac, 6) == 0){
			dev_vdbg("packet detect\n");
		}else{
			continue;
		}
		return 0;
	}
}

static int eth_indaddr_write(const unsigned char *mac)
{
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_SA1, (mac[5] << 8) | mac[4]);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_SA2, (mac[3] << 8) | mac[2]);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_SA3, (mac[1] << 8) | mac[0]);
	return 0;
}

static int eth_reset(void)
{
	unsigned long val;

	trace();

	/* Reset MAC */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1) |
		NS_ETH_EGCR1_MAC_HRST);

	udelay(5);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1) &
		~NS_ETH_EGCR1_MAC_HRST);

	/* Reset and Initialize PHY */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MAC1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_MAC1) &
		~NS_ETH_MAC1_SRST);

	/* Set MDIO Clock */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MCFG, NS_ETH_MCFG_CLKS_40);

	/* Reset PHY */
	val = ns_mii_read(PHY_COMMON_CTRL);
	ns_mii_write(PHY_COMMON_CTRL, val | PHY_COMMON_CTRL_RESET);
	udelay(350);

	/* now take the highest MDIO clock possible after detection */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MCFG, NS_ETH_MCFG_CLKS_40);

	val = ns_mii_read(PHY_COMMON_ID1);
	dev_dbg("phy id1: %p\n", val);
	val = ns_mii_read(PHY_COMMON_ID2);
	dev_dbg("phy id2: %p\n", val);

	return 0;
}

static void ns9750_eth_update_rx_desc(void)
{
	int timeout = 20;

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1) |
		NS_ETH_EGCR1_ERXINIT);

	/* [1] Tab. 221 states less than 5us */
	while(--timeout && !(read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGSR) &
			     NS_ETH_EGSR_RXINIT))
		/* wait for finish */
		udelay(1);

	if(!timeout)
		dev_err("ethernet descriptor update failed\n");

	/* @TODO do we need to clear RXINIT? */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1) &
		~NS_ETH_EGCR1_ERXINIT);

	/* all 4 descriptors free */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_RXFREE, 0xf);
}

static int eth_enable(void)
{
	u32 egcr1;
	trace();

	/* enable hardware */
	/* enable receive and transmit FIFO, use 10/100 Mbps MII */
	egcr1 = (NS_ETH_EGCR1_ETXWM |
		 NS_ETH_EGCR1_ERX |
		 NS_ETH_EGCR1_ETX |
#ifdef NS9750_RXALIGN
		 NS_ETH_EGCR1_RXALIGN |
#endif
		 NS_ETH_EGCR1_PHY_MODE_MII |
		 NS_ETH_EGCR1_ITXA);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1, egcr1);

	/* EINTR can only be reseted with ERX and ETX on */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EINTR,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EINTR));

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MAC1, NS_ETH_MAC1_RXEN);

	/* the linux kernel may give packets < 60 bytes, for example arp */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MAC2,
		NS_ETH_MAC2_CRCEN | NS_ETH_MAC2_PADEN);

	/* program the first descriptors for each ring. */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_RXAPTR,
		RX_DESC_BASE + DESC_SIZE * 0);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_RXBPTR,
		RX_DESC_BASE + DESC_SIZE * 1);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_RXCPTR,
		RX_DESC_BASE + DESC_SIZE * 2);
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_RXDPTR,
		RX_DESC_BASE + DESC_SIZE * 3);

	ns9750_eth_update_rx_desc();

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1) |
		NS_ETH_EGCR1_ERXDMA);

	/* clear statistics */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR2, NS_ETH_EGCR2_CLRCNT);
	/* enable statistics */
	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR2,
		NS_ETH_EGCR2_AUTOZ | NS_ETH_EGCR2_STEN);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_SAFR,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_SAFR) | NS_ETH_SAFR_BROAD);
	return 0;
}

static int queue_init(void)
{
	addr_t rx_desc;
	int i;
	trace();

	for(i=0;i<4;i++){
		rx_desc = RX_DESC_BASE + DESC_SIZE * i;
		write32(rx_desc + DESC_STA, 0x0);
		write32(rx_desc + DESC_DST, 0x0);
		write32(rx_desc + DESC_SRC, (u32)&rxbuf[NS_ETH_FRAME_LEN * i]);
		write32(rx_desc + DESC_LEN, NS_ETH_FRAME_LEN);
		write32(rx_desc + DESC_STA, 0xa0000000);
	}
	return 0;
}

int ns9750_eth_init(struct net_device *dev)
{
	int ret;

	trace();

	dev->enable_phy = ns9750_enable_phy_module;
	dev->disable_phy = ns9750_disable_phy_module;
	dev->send = ns9750_eth_send;
	dev->recv = ns9750_eth_recv;
	get_mac_address(dev->eth_mac);

	enable_phy_module(dev);

	eth_reset();


	ret = phy_init();
	if(ret == -1){
		dev_info_r("Link is down\n");
		return -1;
	}

	eth_indaddr_write(dev->eth_mac);


	queue_init();

	eth_enable();

	return 0;
}

int ns9750_eth_shutdown(struct net_device *dev)
{
	trace();

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_MAC1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_MAC1) & ~NS_ETH_MAC1_RXEN);

	write32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1,
		read32(NS9750_ETH_PHYS_BASE + NS_ETH_EGCR1) &
		~(NS_ETH_EGCR1_ERX | NS_ETH_EGCR1_ERXDMA |
		  NS_ETH_EGCR1_ETX | NS_ETH_EGCR1_ETXDMA));

	return 0;
}

struct net_device ns9750_eth = {
	.init = ns9750_eth_init,
	.exit = ns9750_eth_shutdown,
};
