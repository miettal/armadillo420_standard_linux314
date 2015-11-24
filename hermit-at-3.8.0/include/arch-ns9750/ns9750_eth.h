#ifndef _NS9750_ETH_H_
#define _NS9750_ETH_H_

/* register bit fields */

#define NS_ETH_EGCR1_ERX	 	(0x80000000)
#define NS_ETH_EGCR1_ERXDMA	 	(0x40000000)
#define NS_ETH_EGCR1_ERXSHT	 	(0x10000000)
#define NS_ETH_EGCR1_ERXSIZ	 	(0x08000000)
#define NS_ETH_EGCR1_ETXSIZ	 	(0x04000000)
#define NS_ETH_EGCR1_ETXDIAG		(0x02000000)
#define NS_ETH_EGCR1_ERXBAD	 	(0x01000000)
#define NS_ETH_EGCR1_ETX	 	(0x00800000)
#define NS_ETH_EGCR1_ETXDMA	 	(0x00400000)
#define NS_ETH_EGCR1_ETXWM	  	(0x00200000)
#define NS_ETH_EGCR1_ERXADV	 	(0x00100000)
#define NS_ETH_EGCR1_ERXINIT		(0x00080000)
#define NS_ETH_EGCR1_PHY_MODE_MA  	(0x0000C000)
#define NS_ETH_EGCR1_PHY_MODE_MII 	(0x00000000)
#define NS_ETH_EGCR1_PHY_MODE_RMII 	(0x00004000)
#define NS_ETH_EGCR1_RXCINV	 	(0x00001000)
#define NS_ETH_EGCR1_TXCINV	 	(0x00000800)
#define NS_ETH_EGCR1_RXALIGN		(0x00000400)
#define NS_ETH_EGCR1_MAC_HRST 		(0x00000200)
#define NS_ETH_EGCR1_ITXA	 	(0x00000100)

#define NS_ETH_EGCR2_TPTV_MA		(0xFFFF0000)
#define NS_ETH_EGCR2_TPCF		(0x00000040)
#define NS_ETH_EGCR2_THPDF		(0x00000020)
#define NS_ETH_EGCR2_TCLER		(0x00000008)
#define NS_ETH_EGCR2_AUTOZ		(0x00000004)
#define NS_ETH_EGCR2_CLRCNT		(0x00000002)
#define NS_ETH_EGCR2_STEN		(0x00000001)

#define NS_ETH_EGSR_RXINIT	 	(0x00100000)
#define NS_ETH_EGSR_TXFIFONF 		(0x00080000)
#define NS_ETH_EGSR_TXFIFOH	 	(0x00040000)
#define NS_ETH_EGSR_TXFIFOE	 	(0x00010000)

#define NS_ETH_FIFOTXS_ALL		(0x00000055)
#define NS_ETH_FIFOTXS_3		(0x000000d5)
#define NS_ETH_FIFOTXS_2		(0x00000035)
#define NS_ETH_FIFOTXS_1		(0x0000000D)
#define NS_ETH_FIFOTXS_0		(0x00000003)

#define NS_ETH_ETSR_TXOK	 	(0x00008000)
#define NS_ETH_ETSR_TXBR	 	(0x00004000)
#define NS_ETH_ETSR_TXMC	 	(0x00002000)
#define NS_ETH_ETSR_TXAL	 	(0x00001000)
#define NS_ETH_ETSR_TXAED	 	(0x00000800)
#define NS_ETH_ETSR_TXAEC	 	(0x00000400)
#define NS_ETH_ETSR_TXAUR	 	(0x00000200)
#define NS_ETH_ETSR_TXAJ	 	(0x00000100)
#define NS_ETH_ETSR_TXDEF	 	(0x00000040)
#define NS_ETH_ETSR_TXCRC	 	(0x00000020)
#define NS_ETH_ETSR_TXCOLC   		(0x0000000F)

#define NS_ETH_ERSR_RXSIZE_MA		(0x0FFF0000)
#define NS_ETH_ERSR_RXCE	 	(0x00008000)
#define NS_ETH_ERSR_RXDV	 	(0x00004000)
#define NS_ETH_ERSR_RXOK	 	(0x00002000)
#define NS_ETH_ERSR_RXBR	 	(0x00001000)
#define NS_ETH_ERSR_RXMC	 	(0x00000800)
#define NS_ETH_ERSR_RXCRC	 	(0x00000400)
#define NS_ETH_ERSR_RXDR	 	(0x00000200)
#define NS_ETH_ERSR_RXCV	 	(0x00000100)
#define NS_ETH_ERSR_RXSHT	 	(0x00000040)

#define NS_ETH_MAC1_SRST	 	(0x00008000)
#define NS_ETH_MAC1_SIMMRST	 	(0x00004000)
#define NS_ETH_MAC1_RPEMCSR	 	(0x00000800)
#define NS_ETH_MAC1_RPERFUN	 	(0x00000400)
#define NS_ETH_MAC1_RPEMCST	 	(0x00000200)
#define NS_ETH_MAC1_RPETFUN	 	(0x00000100)
#define NS_ETH_MAC1_LOOPBK	 	(0x00000010)
#define NS_ETH_MAC1_TXFLOW	 	(0x00000008)
#define NS_ETH_MAC1_RXFLOW	 	(0x00000004)
#define NS_ETH_MAC1_PALLRX	 	(0x00000002)
#define NS_ETH_MAC1_RXEN	 	(0x00000001)

#define NS_ETH_MAC2_EDEFER	 	(0x00004000)
#define NS_ETH_MAC2_BACKP	 	(0x00002000)
#define NS_ETH_MAC2_NOBO	 	(0x00001000)
#define NS_ETH_MAC2_LONGP	 	(0x00000200)
#define NS_ETH_MAC2_PUREP	 	(0x00000100)
#define NS_ETH_MAC2_AUTOP	 	(0x00000080)
#define NS_ETH_MAC2_VLANP	 	(0x00000040)
#define NS_ETH_MAC2_PADEN  	 	(0x00000020)
#define NS_ETH_MAC2_CRCEN	 	(0x00000010)
#define NS_ETH_MAC2_DELCRC	 	(0x00000008)
#define NS_ETH_MAC2_HUGE	 	(0x00000004)
#define NS_ETH_MAC2_FLENC	 	(0x00000002)
#define NS_ETH_MAC2_FULLD	 	(0x00000001)

#define NS_ETH_IPGT_MA	 		(0x0000007F)

#define NS_ETH_IPGR_IPGR1	 	(0x00007F00)
#define NS_ETH_IPGR_IPGR2	 	(0x0000007F)

#define NS_ETH_CLRT_CWIN	 	(0x00003F00)
#define	NS_ETH_CLRT_RETX	 	(0x0000000F)

#define NS_ETH_MAXF_MAXF	 	(0x0000FFFF)

#define NS_ETH_SUPP_RPERMII	 	(0x00008000)
#define NS_ETH_SUPP_SPEED  	 	(0x00000080)

#define NS_ETH_TEST_TBACK	 	(0x00000004)
#define NS_ETH_TEST_TPAUSE	 	(0x00000002)
#define NS_ETH_TEST_SPQ	 		(0x00000001)

#define NS_ETH_MCFG_RMIIM	 	(0x00008000)
#define NS_ETH_MCFG_CLKS_MA	 	(0x0000001C)
#define NS_ETH_MCFG_CLKS_4	 	(0x00000004)
#define NS_ETH_MCFG_CLKS_6	 	(0x00000008)
#define NS_ETH_MCFG_CLKS_8	 	(0x0000000C)
#define NS_ETH_MCFG_CLKS_10	 	(0x00000010)
#define NS_ETH_MCFG_CLKS_20	 	(0x00000014)
#define NS_ETH_MCFG_CLKS_30	 	(0x00000018)
#define NS_ETH_MCFG_CLKS_40	 	(0x0000001C)
#define NS_ETH_MCFG_SPRE	 	(0x00000002)
#define NS_ETH_MCFG_SCANI	 	(0x00000001)

#define NS_ETH_MCMD_SCAN	 	(0x00000002)
#define NS_ETH_MCMD_READ	 	(0x00000001)

#define NS_ETH_MADR_DADR_MA	 	(0x00001F00)
#define NS_ETH_MADR_RADR_MA	 	(0x0000001F)

#define NS_ETH_MWTD_MA	 		(0x0000FFFF)

#define NS_ETH_MRRD_MA	 		(0x0000FFFF)

#define NS_ETH_MIND_MIILF		(0x00000008)
#define NS_ETH_MIND_NVALID		(0x00000004)
#define NS_ETH_MIND_SCAN	 	(0x00000002)
#define NS_ETH_MIND_BUSY	 	(0x00000001)

#define NS_ETH_SA1_OCTET1_MA 		(0x0000FF00)
#define NS_ETH_SA1_OCTET2_MA 		(0x000000FF)

#define NS_ETH_SA2_OCTET3_MA 		(0x0000FF00)
#define NS_ETH_SA2_OCTET4_MA 		(0x000000FF)

#define NS_ETH_SA3_OCTET5_MA 		(0x0000FF00)
#define NS_ETH_SA3_OCTET6_MA 		(0x000000FF)

#define NS_ETH_SAFR_PRO	 		(0x00000008)
#define NS_ETH_SAFR_PRM	 		(0x00000004)
#define NS_ETH_SAFR_PRA	 		(0x00000002)
#define NS_ETH_SAFR_BROAD	 	(0x00000001)

#define NS_ETH_HT1_MA	 		(0x0000FFFF)

#define NS_ETH_HT2_MA	 		(0x0000FFFF)

/* also valid for EINTREN */
#define NS_ETH_EINTR_RXOVL_DATA		(0x02000000)
#define NS_ETH_EINTR_RXOVL_STAT		(0x01000000)
#define NS_ETH_EINTR_RXBUFC		(0x00800000)
#define NS_ETH_EINTR_RXDONEA		(0x00400000)
#define NS_ETH_EINTR_RXDONEB		(0x00200000)
#define NS_ETH_EINTR_RXDONEC		(0x00100000)
#define NS_ETH_EINTR_RXDONED		(0x00080000)
#define NS_ETH_EINTR_RXNOBUF		(0x00040000)
#define NS_ETH_EINTR_RXBUFFUL		(0x00020000)
#define NS_ETH_EINTR_RXBR		(0x00010000)
#define NS_ETH_EINTR_STOVFL		(0x00000040)
#define NS_ETH_EINTR_TXBUFC		(0x00000010)
#define NS_ETH_EINTR_TXBUFNR		(0x00000008)
#define NS_ETH_EINTR_TXDONE		(0x00000004)
#define NS_ETH_EINTR_TXERR 		(0x00000002)
#define NS_ETH_EINTR_TXIDLE		(0x00000001)
#define NS_ETH_EINTR_RX_MA	\
	(NS_ETH_EINTR_RXOVL_DATA | \
	 NS_ETH_EINTR_RXOVL_STAT | \
	 NS_ETH_EINTR_RXBUFC | \
	 NS_ETH_EINTR_RXDONEA | \
	 NS_ETH_EINTR_RXDONEB | \
	 NS_ETH_EINTR_RXDONEC | \
	 NS_ETH_EINTR_RXDONED | \
	 NS_ETH_EINTR_RXNOBUF | \
	 NS_ETH_EINTR_RXBUFFUL | \
	 NS_ETH_EINTR_RXBR )
#define NS_ETH_EINTR_TX_MA	\
	(NS_ETH_EINTR_TXBUFC |	\
	 NS_ETH_EINTR_TXBUFNR | \
	 NS_ETH_EINTR_TXDONE | \
	 NS_ETH_EINTR_TXERR | \
	 NS_ETH_EINTR_TXIDLE)

/* for TXPTR, TXRPTR, TXERBD and TXSPTR */
#define NS_ETH_TXPTR_MA			(0x000000FF)

/* for RXAOFF, RXBOFF, RXCOFF and RXDOFF */
#define NS_ETH_RXOFF_MA			(0x000007FF)

#define NS_ETH_TXOFF_MA			(0x000003FF)

#define NS_ETH_RXFREE_D			(0x00000008)
#define NS_ETH_RXFREE_C			(0x00000004)
#define NS_ETH_RXFREE_B			(0x00000002)
#define NS_ETH_RXFREE_A			(0x00000001)

/* PHY definitions (LXT971A) [2] */

#define PHY_COMMON_CTRL    	 	(0x00)
#define PHY_COMMON_STAT    	 	(0x01)
#define PHY_COMMON_ID1    	 	(0x02)
#define PHY_COMMON_ID2           	(0x03)
#define PHY_COMMON_AUTO_ADV      	(0x04)
#define PHY_COMMON_AUTO_LNKB     	(0x05)
#define PHY_COMMON_AUTO_EXP      	(0x06)
#define PHY_COMMON_AUTO_NEXT     	(0x07)
#define PHY_COMMON_AUTO_LNKN     	(0x08)
#define PHY_LXT971_PORT_CFG      	(0x10)
#define PHY_LXT971_STAT2         	(0x11)
#define PHY_LXT971_INT_ENABLE    	(0x12)
#define PHY_LXT971_INT_STATUS    	(0x13)
#define PHY_LXT971_LED_CFG       	(0x14)
#define PHY_LXT971_DIG_CFG       	(0x1A)
#define PHY_LXT971_TX_CTRL       	(0x1E)

/* CTRL PHY Control Register Bit Fields */

#define PHY_COMMON_CTRL_RESET  	 	(0x8000)
#define PHY_COMMON_CTRL_LOOPBACK 	(0x4000)
#define PHY_COMMON_CTRL_SPD_MA   	(0x2040)
#define PHY_COMMON_CTRL_SPD_10   	(0x0000)
#define PHY_COMMON_CTRL_SPD_100  	(0x2000)
#define PHY_COMMON_CTRL_SPD_1000 	(0x0040)
#define PHY_COMMON_CTRL_SPD_RES  	(0x2040)
#define PHY_COMMON_CTRL_AUTO_NEG 	(0x1000)
#define PHY_COMMON_CTRL_POWER_DN 	(0x0800)
#define PHY_COMMON_CTRL_ISOLATE	 	(0x0400)
#define PHY_COMMON_CTRL_RES_AUTO 	(0x0200)
#define PHY_COMMON_CTRL_DUPLEX	 	(0x0100)
#define PHY_COMMON_CTRL_COL_TEST 	(0x0080)
#define PHY_COMMON_CTRL_RES1     	(0x003F)

/* STAT Status Register Bit Fields */

#define PHY_COMMON_STAT_100BT4	 	(0x8000)
#define PHY_COMMON_STAT_100BXFD	 	(0x4000)
#define PHY_COMMON_STAT_100BXHD	 	(0x2000)
#define PHY_COMMON_STAT_10BTFD	 	(0x1000)
#define PHY_COMMON_STAT_10BTHD	 	(0x0800)
#define PHY_COMMON_STAT_100BT2FD 	(0x0400)
#define PHY_COMMON_STAT_100BT2HD 	(0x0200)
#define PHY_COMMON_STAT_EXT_STAT 	(0x0100)
#define PHY_COMMON_STAT_RES1	 	(0x0080)
#define PHY_COMMON_STAT_MF_PSUP	 	(0x0040)
#define PHY_COMMON_STAT_AN_COMP  	(0x0020)
#define PHY_COMMON_STAT_RMT_FLT	 	(0x0010)
#define PHY_COMMON_STAT_AN_CAP	 	(0x0008)
#define PHY_COMMON_STAT_LNK_STAT 	(0x0004)
#define PHY_COMMON_STAT_JAB_DTCT 	(0x0002)
#define PHY_COMMON_STAT_EXT_CAP	 	(0x0001)


/* AUTO_ADV Auto-neg Advert Register Bit Fields */

#define PHY_COMMON_AUTO_ADV_NP       	(0x8000)
#define PHY_COMMON_AUTO_ADV_RES1        (0x4000)
#define PHY_COMMON_AUTO_ADV_RMT_FLT     (0x2000)
#define PHY_COMMON_AUTO_ADV_RES2        (0x1000)
#define PHY_COMMON_AUTO_ADV_AS_PAUSE    (0x0800)
#define PHY_COMMON_AUTO_ADV_PAUSE       (0x0400)
#define PHY_COMMON_AUTO_ADV_100BT4      (0x0200)
#define PHY_COMMON_AUTO_ADV_100BTXFD   	(0x0100)
#define PHY_COMMON_AUTO_ADV_100BTX      (0x0080)
#define PHY_COMMON_AUTO_ADV_10BTFD   	(0x0040)
#define PHY_COMMON_AUTO_ADV_10BT     	(0x0020)
#define PHY_COMMON_AUTO_ADV_SEL_FLD_MA  (0x001F)
#define PHY_COMMON_AUTO_ADV_802_9       (0x0002)
#define PHY_COMMON_AUTO_ADV_802_3       (0x0001)

/* AUTO_LNKB Auto-neg Link Ability Register Bit Fields */

#define PHY_COMMON_AUTO_LNKB_NP       	(0x8000)
#define PHY_COMMON_AUTO_LNKB_ACK        (0x4000)
#define PHY_COMMON_AUTO_LNKB_RMT_FLT    (0x2000)
#define PHY_COMMON_AUTO_LNKB_RES2       (0x1000)
#define PHY_COMMON_AUTO_LNKB_AS_PAUSE   (0x0800)
#define PHY_COMMON_AUTO_LNKB_PAUSE      (0x0400)
#define PHY_COMMON_AUTO_LNKB_100BT4     (0x0200)
#define PHY_COMMON_AUTO_LNKB_100BTXFD   (0x0100)
#define PHY_COMMON_AUTO_LNKB_100BTX     (0x0080)
#define PHY_COMMON_AUTO_LNKB_10BTFD   	(0x0040)
#define PHY_COMMON_AUTO_LNKB_10BT     	(0x0020)
#define PHY_COMMON_AUTO_LNKB_SEL_FLD_MA (0x001F)
#define PHY_COMMON_AUTO_LNKB_802_9      (0x0002)
#define PHY_COMMON_AUTO_LNKB_802_3      (0x0001)

/* AUTO_EXP Auto-neg Expansion Register Bit Fields */

#define PHY_COMMON_AUTO_EXP_RES1        (0xFFC0)
#define PHY_COMMON_AUTO_EXP_BASE_PAGE   (0x0020)
#define PHY_COMMON_AUTO_EXP_PAR_DT_FLT  (0x0010)
#define PHY_COMMON_AUTO_EXP_LNK_NP_CAP  (0x0008)
#define PHY_COMMON_AUTO_EXP_NP_CAP      (0x0004)
#define PHY_COMMON_AUTO_EXP_PAGE_REC    (0x0002)
#define PHY_COMMON_AUTO_EXP_LNK_AN_CAP  (0x0001)

/* AUTO_NEXT Aut-neg Next Page Tx Register Bit Fields */

#define PHY_COMMON_AUTO_NEXT_NP         (0x8000)
#define PHY_COMMON_AUTO_NEXT_RES1       (0x4000)
#define PHY_COMMON_AUTO_NEXT_MSG_PAGE   (0x2000)
#define PHY_COMMON_AUTO_NEXT_ACK_2      (0x1000)
#define PHY_COMMON_AUTO_NEXT_TOGGLE     (0x0800)
#define PHY_COMMON_AUTO_NEXT_MSG        (0x07FF)

/* AUTO_LNKN Auto-neg Link Partner Rx Reg Bit Fields */

#define PHY_COMMON_AUTO_LNKN_NP         (0x8000)
#define PHY_COMMON_AUTO_LNKN_ACK        (0x4000)
#define PHY_COMMON_AUTO_LNKN_MSG_PAGE   (0x2000)
#define PHY_COMMON_AUTO_LNKN_ACK_2      (0x1000)
#define PHY_COMMON_AUTO_LNKN_TOGGLE     (0x0800)
#define PHY_COMMON_AUTO_LNKN_MSG        (0x07FF)

/* PORT_CFG Port Configuration Register Bit Fields */

#define PHY_LXT971_PORT_CFG_RES1        (0x8000)
#define PHY_LXT971_PORT_CFG_FORCE_LNK   (0x4000)
#define PHY_LXT971_PORT_CFG_TX_DISABLE  (0x2000)
#define PHY_LXT971_PORT_CFG_BYPASS_SCR  (0x1000)
#define PHY_LXT971_PORT_CFG_RES2        (0x0800)
#define PHY_LXT971_PORT_CFG_JABBER      (0x0400)
#define PHY_LXT971_PORT_CFG_SQE	        (0x0200)
#define PHY_LXT971_PORT_CFG_TP_LOOPBACK (0x0100)
#define PHY_LXT971_PORT_CFG_CRS_SEL     (0x0080)
#define PHY_LXT971_PORT_CFG_SLEEP_MODE  (0x0040)
#define PHY_LXT971_PORT_CFG_PRE_EN      (0x0020)
#define PHY_LXT971_PORT_CFG_SLEEP_T_MA  (0x0018)
#define PHY_LXT971_PORT_CFG_SLEEP_T_104 (0x0010)
#define PHY_LXT971_PORT_CFG_SLEEP_T_200 (0x0001)
#define PHY_LXT971_PORT_CFG_SLEEP_T_304 (0x0000)
#define PHY_LXT971_PORT_CFG_FLT_CODE_EN (0x0004)
#define PHY_LXT971_PORT_CFG_ALT_NP      (0x0002)
#define PHY_LXT971_PORT_CFG_FIBER_SEL   (0x0001)

/* STAT2 Status Register #2 Bit Fields */

#define PHY_LXT971_STAT2_RES1   	(0x8000)
#define PHY_LXT971_STAT2_100BTX 	(0x4000)
#define PHY_LXT971_STAT2_TX_STATUS	(0x2000)
#define PHY_LXT971_STAT2_RX_STATUS	(0x1000)
#define PHY_LXT971_STAT2_COL_STATUS	(0x0800)
#define PHY_LXT971_STAT2_LINK   	(0x0400)
#define PHY_LXT971_STAT2_DUPLEX_MODE	(0x0200)
#define PHY_LXT971_STAT2_AUTO_NEG	(0x0100)
#define PHY_LXT971_STAT2_AUTO_NEG_COMP 	(0x0080)
#define PHY_LXT971_STAT2_RES2   	(0x0040)
#define PHY_LXT971_STAT2_POLARITY	(0x0020)
#define PHY_LXT971_STAT2_PAUSE  	(0x0010)
#define PHY_LXT971_STAT2_ERROR  	(0x0008)
#define PHY_LXT971_STAT2_RES3   	(0x0007)

/* INT_ENABLE Interrupt Enable Register Bit Fields */

#define PHY_LXT971_INT_ENABLE_RES1      (0xFF00)
#define PHY_LXT971_INT_ENABLE_ANMSK     (0x0080)
#define PHY_LXT971_INT_ENABLE_SPEEDMSK  (0x0040)
#define PHY_LXT971_INT_ENABLE_DUPLEXMSK (0x0020)
#define PHY_LXT971_INT_ENABLE_LINKMSK   (0x0010)
#define PHY_LXT971_INT_ENABLE_RES2      (0x000C)
#define PHY_LXT971_INT_ENABLE_INTEN     (0x0002)
#define PHY_LXT971_INT_ENABLE_TINT      (0x0001)

/* INT_STATUS Interrupt Status Register Bit Fields */

#define PHY_LXT971_INT_STATUS_RES1      (0xFF00)
#define PHY_LXT971_INT_STATUS_ANDONE    (0x0080)
#define PHY_LXT971_INT_STATUS_SPEEDCHG  (0x0040)
#define PHY_LXT971_INT_STATUS_DUPLEXCHG (0x0020)
#define PHY_LXT971_INT_STATUS_LINKCHG   (0x0010)
#define PHY_LXT971_INT_STATUS_RES2      (0x0008)
#define PHY_LXT971_INT_STATUS_MDINT     (0x0004)
#define PHY_LXT971_INT_STATUS_RES3      (0x0003)

/* LED_CFG Interrupt LED Configuration Register Bit Fields */

#define PHY_LXT971_LED_CFG_SHIFT_LED1   (0x000C)
#define PHY_LXT971_LED_CFG_SHIFT_LED2   (0x0008)
#define PHY_LXT971_LED_CFG_SHIFT_LED3   (0x0004)
#define PHY_LXT971_LED_CFG_LEDFREQ_MA	(0x000C)
#define PHY_LXT971_LED_CFG_LEDFREQ_RES	(0x000C)
#define PHY_LXT971_LED_CFG_LEDFREQ_100	(0x0008)
#define PHY_LXT971_LED_CFG_LEDFREQ_60	(0x0004)
#define PHY_LXT971_LED_CFG_LEDFREQ_30	(0x0000)
#define PHY_LXT971_LED_CFG_PULSE_STR    (0x0002)
#define PHY_LXT971_LED_CFG_RES1         (0x0001)

/* only one of these values must be shifted for each SHIFT_LED?  */

#define PHY_LXT971_LED_CFG_UNUSED1      (0x000F)
#define PHY_LXT971_LED_CFG_DUPLEX_COL   (0x000E)
#define PHY_LXT971_LED_CFG_LINK_ACT     (0x000D)
#define PHY_LXT971_LED_CFG_LINK_RX      (0x000C)
#define PHY_LXT971_LED_CFG_TEST_BLK_SLW (0x000B)
#define PHY_LXT971_LED_CFG_TEST_BLK_FST (0x000A)
#define PHY_LXT971_LED_CFG_TEST_OFF     (0x0009)
#define PHY_LXT971_LED_CFG_TEST_ON      (0x0008)
#define PHY_LXT971_LED_CFG_RX_OR_TX     (0x0007)
#define PHY_LXT971_LED_CFG_UNUSED2      (0x0006)
#define PHY_LXT971_LED_CFG_DUPLEX       (0x0005)
#define PHY_LXT971_LED_CFG_LINK	        (0x0004)
#define PHY_LXT971_LED_CFG_COLLISION    (0x0003)
#define PHY_LXT971_LED_CFG_RECEIVE      (0x0002)
#define PHY_LXT971_LED_CFG_TRANSMIT     (0x0001)
#define PHY_LXT971_LED_CFG_SPEED        (0x0000)

/* DIG_CFG Digitial Configuration Register Bit Fields */

#define PHY_LXT971_DIG_CFG_RES1 	(0xF000)
#define PHY_LXT971_DIG_CFG_MII_DRIVE	(0x0800)
#define PHY_LXT971_DIG_CFG_RES2 	(0x0400)
#define PHY_LXT971_DIG_CFG_SHOW_SYMBOL	(0x0200)
#define PHY_LXT971_DIG_CFG_RES3 	(0x01FF)

#define PHY_LXT971_MDIO_MAX_CLK		(8000000)

/* ICS1893BK PHY Stuff */
/* Extended Control Register */
#define PHY_ICS1893_EXT_CTRL_CMD_OWR		(0x8000)
#define PHY_ICS1893_EXT_CTRL_MONDL_4		(0x0400)
#define PHY_ICS1893_EXT_CTRL_MONDL_3		(0x0200)
#define PHY_ICS1893_EXT_CTRL_MONDL_2		(0x0100)
#define PHY_ICS1893_EXT_CTRL_MONDL_1		(0x0080)
#define PHY_ICS1893_EXT_CTRL_MONDL_0		(0x0040)
#define PHY_ICS1893_EXT_CTRL_MONDL		(0x07C0)
#define PHY_ICS1893_EXT_CTRL_SCIPHER_TEST	(0x0020)
#define PHY_ICS1893_EXT_CTRL_ICS_RES2		(0x0010)
#define PHY_ICS1893_EXT_CTRL_NRZ_NRZI		(0x0008)
#define PHY_ICS1893_EXT_CTRL_TX_INV		(0x0004)
#define PHY_ICS1893_EXT_CTRL_ICS_RES1		(0x0002)
#define PHY_ICS1893_EXT_CTRL_SCIPHER		(0x0001)

/* QuickPoll Detailed Status Register */
#define PHY_ICS1893_QPSTAT_DATA_RATE			(0x8000)
#define PHY_ICS1893_QPSTAT_DUPLEX			(0x4000)
#define PHY_ICS1893_QPSTAT_AUTO_NEG_PROG_2	(0x2000)
#define PHY_ICS1893_QPSTAT_AUTO_NEG_PROG_1	(0x1000)
#define PHY_ICS1893_QPSTAT_AUTO_NEG_PROG_0	(0x0800)
#define PHY_ICS1893_QPSTAT_AUTO_NEG_PROG		(0x3800)
#define PHY_ICS1893_QPSTAT_100TX_LOST			(0x0400)
#define PHY_ICS1893_QPSTAT_100PLL_LOCK 		(0x0200)
#define PHY_ICS1893_QPSTAT_FALSE_CAR_DETECT   	(0x0100)
#define PHY_ICS1893_QPSTAT_INV_DETECT			(0x0080)
#define PHY_ICS1893_QPSTAT_HALT_DETECT		(0x0040)
#define PHY_ICS1893_QPSTAT_PRE_END_DETECT 	(0x0020)
#define PHY_ICS1893_QPSTAT_AUTO_NEG_COMP   	(0x0010)
#define PHY_ICS1893_QPSTAT_100BTX			(0x0008)
#define PHY_ICS1893_QPSTAT_JDETECT  			(0x0004)
#define PHY_ICS1893_QPSTAT_REMFAULT  			(0x0002)
#define PHY_ICS1893_QPSTAT_LINKSTAT   			(0x0001)

/* I10Base-T Operations Register */
#define PHY_ICS1893_10B_OPSTAT_REM_JAB_DTCT		(0x8000)
#define PHY_ICS1893_10B_OPSTAT_POL_RES			(0x4000)
#define PHY_ICS1893_10B_OPSTAT_JAB_INHIB			(0x0020)
#define PHY_ICS1893_10B_OPSTAT_AUTOPOL_INHIB		(0x0008)
#define PHY_ICS1893_10B_OPSTAT_SQE_INHIB			(0x0004)
#define PHY_ICS1893_10B_OPSTAT_LLOSS_INHIB		(0x0002)
#define PHY_ICS1893_10B_OPSTAT_SQUELCH_INHIB		(0x0001)

/* Extended Control Register 2 */
#define PHY_ICS1893_EXT_CTRL_2_RES1      (0xFF00)
#define PHY_ICS1893_EXT_CTRL_2_NODE_MODE			(0x8000)
#define PHY_ICS1893_EXT_CTRL_2_HW_SW_MODE		(0x4000)
#define PHY_ICS1893_EXT_CTRL_2_REM_FAULT			(0x2000)
#define PHY_ICS1893_EXT_CTRL_2_AMDIX_EN			(0x0200)
#define PHY_ICS1893_EXT_CTRL_2_MDI_MODE			(0x0100)
#define PHY_ICS1893_EXT_CTRL_2_TP_TRISTATE			(0x0080)
#define PHY_ICS1893_EXT_CTRL_2_AUTO_100BTX_DWN	(0x0001)

#define PHY_ICS1893_MDIO_MAX_CLK		(25000000)

/* TX_CTRL Transmit Control Register Bit Fields
   documentation is buggy for this register, therefore setting not included */

#define PHY_ID_MASK 0xfffffff0

#ifndef __ASSEMBLER__
typedef enum 
{
	PHY_NONE	= 0x00000000, /* no PHY detected yet */
	PHY_LXT971A	= 0x001378e0,
	PHY_ICS1893BK	= 0x0015f440,
} phy_type_t;
#endif

#define PHY_MDIO_MAX_CLK		(2500000)

#endif