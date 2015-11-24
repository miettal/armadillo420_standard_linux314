#define CORE_NAME "eth_ep93xx"

#include <autoconf.h>
#if defined(CONFIG_ETHERNET_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <arch/ioregs.h>
#include <arch/regs_eth.h>
#include <io.h>
#include <string.h>
#include <delay.h>
#include <mac.h>
#include <net/net.h>
#include <net/eth.h>
#include <net/eth_util.h>

#include "eth_ep93xx.h"

#define LEN_RxBuf (1518+2+16)
#define LEN_TxBuf (1518+2+16)

static unsigned char *RxBuf = (unsigned char *)0xc4900000;
static unsigned char *TxBuf = (unsigned char *)0xc4a00000;

static ep93xxEth_info *eth_info = (ep93xxEth_info *)0xc4800000;

void ep93xx_enable_phy_module(struct net_device *dev) {
	write32(GPIO_PGDDR, read32(GPIO_PGDDR) | BIT(2));
	write32(GPIO_PGDR, read32(GPIO_PGDR) & ~BIT(2));
}

void ep93xx_disable_phy_module(struct net_device *dev) {
	write32(GPIO_PGDDR, read32(GPIO_PGDDR) | BIT(2));
	write32(GPIO_PGDR, read32(GPIO_PGDR) | BIT(2));
}

static void phy_write(int reg, unsigned short val){

}

static unsigned short phy_read(int reg){
	unsigned short val;
	write32(MAC_SELFCTL, read32(MAC_SELFCTL) & ~BIT(8));

	while(read32(MAC_MIISTS) & (1<<0));

	write32(MAC_MIICMD, 0x8000 | reg);

	while(read32(MAC_MIISTS) & BIT(0));

	val = read32(MAC_MIIDATA);

	write32(MAC_SELFCTL, read32(MAC_SELFCTL) | BIT(8));

	return val;
}

static int phy_auto_negotiation(void){
	unsigned short val;

	phy_write(4, 0x01e1);
	phy_write(0, 0x1200);

	while(1){
		val = phy_read(1);
		if(val & 0x0020){
			break;
		}else{
			mdelay(100);
			continue;
		}
	}

	val = phy_read(5);
	if(val & 0x0140){
		write32(MAC_TESTCTL, read32(MAC_TESTCTL) | TESTCTL_MFDX);
	}
	return 0;
}

static int phy_init(void){
	unsigned short val;
	int ret;

	val = phy_read(1);
	if(val & 0x0004){
		ret = phy_auto_negotiation();
		return ret;
	}
	return -1;
}

int ep93xx_eth_send(struct net_device *dev, struct net_packet *pkt) {
	int idx;
	unsigned char *txBuf;
	int txLen;

	idx = eth_info->idxQueTxDesc;

	txBuf = (unsigned char *)(TxBuf + LEN_TxBuf * idx);

	memcpy(txBuf, pkt->eth, ETH_FRAME_LEN);
	txLen = ETH_FRAME_LEN;
	memcpy(txBuf + txLen, pkt->f2.any, pkt->f2len);
	txLen += pkt->f2len;
	memcpy(txBuf + txLen, pkt->f3.any, pkt->f3len);
	txLen += pkt->f3len;
	memcpy(txBuf + txLen, pkt->f4, pkt->f4len);
	txLen += pkt->f4len;

	if (txLen < 60) {
		memset(txBuf + txLen, 0, 60 - txLen);
		txLen = 60;
	}

	if(txLen > LEN_TxBuf){
		dev_err("not imprement\n");
		return -1;
	}

	eth_info->QueTxDesc[idx].bl = txLen;
	eth_info->QueTxDesc[idx].ba = (unsigned long)txBuf;
	eth_info->QueTxDesc[idx].bi = idx;
	eth_info->QueTxDesc[idx].af = 0;
	eth_info->QueTxDesc[idx].eof = 1;

	dev_dbg("QueTxDesc: %p\n", &eth_info->QueTxDesc);
	dev_dbg("QueTxSts : %p\n", &eth_info->QueTxSts);

	dev_dbg("Desc.ba: %p\n", eth_info->QueTxDesc[idx].ba);
	dev_dbg("Desc.bl: %p\n", eth_info->QueTxDesc[idx].bl);
	dev_dbg("Desc.af: %p\n", eth_info->QueTxDesc[idx].af);
	dev_dbg("Desc.bi: %p\n", eth_info->QueTxDesc[idx].bi);
	dev_dbg("Desc.eof: %p\n", eth_info->QueTxDesc[idx].eof);

	memset(&eth_info->QueTxSts[idx], 0, sizeof(TxStatus));

	write32(MAC_TXDEQ, 1);
	eth_info->idxQueTxDesc = ((eth_info->idxQueTxDesc + 1) % LEN_QueTxDesc);

	while(!(eth_info->QueTxSts[idx].txfp)){
		udelay(1);
	}

	dev_dbg("Sts.bi: %p\n", eth_info->QueTxSts[idx].bi);
	dev_dbg("Sts.ncoll: %p\n", eth_info->QueTxSts[idx].ncoll);
	dev_dbg("Sts.ecoll: %p\n", eth_info->QueTxSts[idx].ecoll);
	dev_dbg("Sts.txu: %p\n", eth_info->QueTxSts[idx].txu);
	dev_dbg("Sts.ow: %p\n", eth_info->QueTxSts[idx].ow);
	dev_dbg("Sts.lcrs: %p\n", eth_info->QueTxSts[idx].lcrs);
	dev_dbg("Sts.fa: %p\n", eth_info->QueTxSts[idx].fa);
	dev_dbg("Sts.txwe: %p\n", eth_info->QueTxSts[idx].txwe);
	dev_dbg("Sts.txfp: %p\n", eth_info->QueTxSts[idx].txfp);

	return 0;
}

int ep93xx_eth_recv(struct net_device *dev, struct net_packet *pkt, int *timeout) {
	int idx;
	int i;

	for(;;){
		idx = eth_info->idxQueRxSts;

		while (--(*timeout)) {
			i = 1000;
			while (i--) {
				if (eth_info->QueRxSts[idx].rfp1)
					goto rx_ready;
				udelay(1);
			}
		}
		if (*timeout == 0)
			return -1;
 rx_ready:

		eth_info->idxQueRxSts = ((eth_info->idxQueRxSts + 1) % LEN_QueRxSts);

		//operated
		eth_info->QueRxSts[idx].rfp1 = 0;
		eth_info->QueRxSts[idx].rfp2 = 0;

		//
		memcpy(pkt->raw_data_addr, (void *)eth_info->QueRxDesc[idx].ba,
		       eth_info->QueRxSts[idx].fl);
		pkt->raw_data_len = eth_info->QueRxSts[idx].fl;
		pkt->eth = pkt->raw_data_addr;

		/* free RxDesc */
		write32(MAC_RXSEQ, 1);
		write32(MAC_RXDEQ, 1);

		//
		if(memcmp(pkt->eth->dmac, broadcast_mac, 6) == 0){
			dev_dbg("broadcast packet detect\n");
		}else if(memcmp(pkt->eth->dmac, dev->eth_mac, 6) == 0){
			dev_dbg("packet detect\n");
		}else{
			continue;
		}
		return 0;
	}
}

static int eth_enable(void){
	write32(MAC_RXCTL, read32(MAC_RXCTL) | RXCTL_SRxON | RXCTL_RCRCA);
	return 0;
}

static int eth_reset(int timeout){
	unsigned short val;
	int i;

	write32(MAC_SELFCTL, SELFCTL_RESET);
	while((read32(MAC_SELFCTL) & SELFCTL_RESET));

	write32(MAC_SELFCTL, (read32(MAC_SELFCTL) & ~0x7f00) | 0x2900);

	for(i=0;i<timeout;i++){
		val = phy_read(1);
		if(val & 0x0004){
			break;
		}
		mdelay(50);
	}

	return 0;
}

static int queue_init(void){
	int i;

	write32(MAC_BMCTL, read32(MAC_BMCTL) | BMCTL_RxDis | BMCTL_TxDis);

	while(read32(MAC_BMSTS) & BMSTS_TxAct){
		mdelay(1000);
	}

	while(read32(MAC_BMSTS) & BMSTS_RxAct);

	memset(&eth_info->QueTxSts, 0, sizeof(TxStatus) * LEN_QueTxSts);
	eth_info->idxQueTxSts = 0;
	write32(MAC_TXSBA, (unsigned long)&eth_info->QueTxSts);
	write32(MAC_TXSCA, (unsigned long)&eth_info->QueTxSts);
	write32(MAC_TXSBL, sizeof(TxStatus) * LEN_QueTxSts);
	write32(MAC_TXSCL, sizeof(TxStatus) * LEN_QueTxSts);

	memset(&eth_info->QueTxDesc, 0, sizeof(TxDescriptor) * LEN_QueTxDesc);
	eth_info->idxQueTxDesc = 0;
	write32(MAC_TXDBA, (unsigned long)&eth_info->QueTxDesc);
	write32(MAC_TXDCA, (unsigned long)&eth_info->QueTxDesc);
	write16(MAC_TXDBL, sizeof(TxDescriptor) * LEN_QueTxDesc);
	write16(MAC_TXDCL, sizeof(TxDescriptor) * LEN_QueTxDesc);

	memset(&eth_info->QueRxSts, 0, sizeof(RxStatus) * LEN_QueRxSts);
	eth_info->idxQueRxSts = 0;
	write32(MAC_RXSBA, (unsigned long)&eth_info->QueRxSts);
	write32(MAC_RXSCA, (unsigned long)&eth_info->QueRxSts);
	write16(MAC_RXSBL, sizeof(RxStatus) * LEN_QueRxSts);
	write16(MAC_RXSCL, sizeof(RxStatus) * LEN_QueRxSts);

	memset(&eth_info->QueRxDesc, 0, sizeof(RxDescriptor) * LEN_QueRxDesc);
	for(i=0; i<LEN_QueRxDesc; i++){
		eth_info->QueRxDesc[i].bi = i;
		eth_info->QueRxDesc[i].ba = (unsigned long)(RxBuf + LEN_RxBuf * i);
		eth_info->QueRxDesc[i].bl = LEN_RxBuf;
	}
	eth_info->idxQueRxDesc = 0;
	write32(MAC_RXDBA, (unsigned long)&eth_info->QueRxDesc);
	write32(MAC_RXDCA, (unsigned long)&eth_info->QueRxDesc);
	write16(MAC_RXDBL, sizeof(RxDescriptor) * LEN_QueRxDesc);
	write16(MAC_RXDCL, sizeof(RxDescriptor) * LEN_QueRxDesc);

	write32(MAC_BMCTL, read32(MAC_BMCTL) | BMCTL_TxEn | BMCTL_RxEn);

	while(!(read32(MAC_BMSTS) & BMSTS_TxAct)){
		mdelay(1000);
	}

	while(!(read32(MAC_BMSTS) & BMSTS_RxAct)){
		mdelay(1000);
	}

	write32(MAC_RXSEQ, LEN_QueRxSts);
	write32(MAC_RXDEQ, LEN_QueRxDesc);

	return 0;
}

static int eth_indaddr_write(const unsigned char *mac){
	unsigned long tmp;

	tmp = read32(MAC_RXCTL);
	write32(MAC_RXCTL, read32(MAC_RXCTL) & ~RXCTL_SRxON);
	write32(MAC_AFP, 0);

	write8(MAC_INDAD + 0, mac[0]);
	write8(MAC_INDAD + 1, mac[1]);
	write8(MAC_INDAD + 2, mac[2]);
	write8(MAC_INDAD + 3, mac[3]);
	write8(MAC_INDAD + 4, mac[4]);
	write8(MAC_INDAD + 5, mac[5]);

	write32(MAC_RXCTL, tmp);
	return 0;
}

int ep93xx_eth_init(struct net_device *dev) {
	int ret;
	unsigned long tmp;

	dev_dbg("eth_info: %p\n", eth_info);
	dev_dbg("QueRxDesc: %p\n", &eth_info->QueRxDesc);
	dev_dbg("QueRxSts: %p\n", &eth_info->QueRxSts);
	dev_dbg("RxBuf: %p\n", RxBuf);
	dev_dbg("TxBuf: %p\n", TxBuf);

	dev->enable_phy = ep93xx_enable_phy_module;
	dev->disable_phy = ep93xx_disable_phy_module;
	dev->send = ep93xx_eth_send;
	dev->recv = ep93xx_eth_recv;
	get_mac_address(dev->eth_mac);

	enable_phy_module(dev);

	eth_reset(100);

	ret = phy_init();
	if(ret == -1){
		dev_info_r("Link is down\n");
		return -1;
	}

	write32(MAC_GLINTMSK, 0x00);
	write32(MAC_RXCTL, RXCTL_BA | RXCTL_IA0);
	write32(MAC_TXCTL, 0x00);
	write32(MAC_GT, 0x00);
	write32(MAC_BMCTL, 0x00);
	write32(MAC_RXBTH, 0x00800040);
	write32(MAC_TXBTH, 0x00800040);
	write32(MAC_RXSTH, 0x00040002);
	write32(MAC_TXSTH, 0x00040002);
	write32(MAC_RXDTH, 0x00040002);
	write32(MAC_TXDTH, 0x00040002);
	write32(MAC_MAXFL, ((1518 + 1) << 16) | (944 << 0));

	tmp = read32(MAC_TXCOLLCNT);
	tmp = read32(MAC_RXMISSCNT);
	tmp = read32(MAC_RXRUNTCNT);

	tmp = read32(MAC_INTSTSC);

	write32(MAC_TXCTL, read32(MAC_TXCTL) | TXCTL_STxON);

	eth_indaddr_write(dev->eth_mac);

	queue_init();

	eth_enable();

	return 0;
}

struct net_device ep93xx_eth = {
	.init = ep93xx_eth_init,
};
