#ifndef _HERMIT_ETH_EP93XX_H_
#define _HERMIT_ETH_EP93XX_H_

/* Length define */
#define LEN_QueRxDesc 32
#define LEN_QueRxSts  LEN_QueRxDesc
#define LEN_QueTxDesc 8
#define LEN_QueTxSts  LEN_QueTxDesc

typedef struct _RxDescriptor {
	unsigned long ba:32,
		      bl:16,
		      bi:15,
		      nsof:1;
} RxDescriptor;

typedef struct _RxStatus {
	unsigned long rsrv1:8,
		      hti:6,
		      rsrv2:1,
		      crci:1,
		      crce:1,
		      edata:1,
		      runt:1,
		      fe:1,
		      oe:1,
		      rxerr:1,
		      am:2,
		      rsrv3:4,
		      eob:1,
		      eof:1,
		      rwe:1,
		      rfp1:1,
		      fl:16,
		      bi:15,
		      rfp2:1;
} RxStatus;

typedef struct _TxDescriptor {
	unsigned long ba:32,
		      bl:12,
		      resv1:3,
		      af:1,
		      bi:15,
		      eof:1;
} TxDescriptor;

typedef struct _TxStatus {
	unsigned long bi:15,
		      rsrv1:1,
		      ncoll:5,
		      rsrv2:3,
		      ecoll:1,
		      txu:1,
		      ow:1,
		      rsrv3:1,
		      lcrs:1,
		      fa:1,
		      txwe:1,
		      txfp:1;
} TxStatus;

typedef struct _ep93xxEth_info {
	/* Queue */
	RxDescriptor QueRxDesc[LEN_QueRxDesc];
	RxStatus     QueRxSts[LEN_QueRxSts];
	TxDescriptor QueTxDesc[LEN_QueTxDesc];
	TxStatus     QueTxSts[LEN_QueTxSts];

	int idxQueRxDesc;
	int idxQueRxSts;
	int idxQueTxDesc;
	int idxQueTxSts;
} ep93xxEth_info;

extern void ep93xx_enable_phy_module(struct net_device *dev);
extern void ep93xx_disable_phy_module(struct net_device *dev);

extern int ep93xx_eth_send(struct net_device *dev, struct net_packet *pkt);
extern int ep93xx_eth_recv(struct net_device *dev, struct net_packet *pkt, int *timeout);
extern int ep93xx_eth_rxbuf_free(struct net_device *dev, const int idx);

#endif
