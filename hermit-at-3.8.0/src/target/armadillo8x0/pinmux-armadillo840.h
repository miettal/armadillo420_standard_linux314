#ifndef _HERMIT_TARGET_PLATFORM_PIMUX_ARMADILLO840_H_
#define _HERMIT_TARGET_PLATFORM_PIMUX_ARMADILLO840_H_

#define PIN_FN_GPIO_PORT16	_PIN_FN(PIN_FMSOOLR,	MUX(0, 0, 1, PULL_OFF))
#define PIN_FN_GPIO_PORT17	_PIN_FN(PIN_FMSOOBT,	MUX(0, 0, 1, PULL_OFF))
#define PIN_FN_GPIO_PORT18	_PIN_FN(PIN_FMSOSLD,	MUX(0, 1, 0, PULL_OFF))
#define PIN_FN_GPIO_PORT89	_PIN_FN(PIN_DREQ_0_N,	MUX(0, 0, 1, PULL_OFF))
#define PIN_FN_GPIO_PORT90	_PIN_FN(PIN_DACK_0_N,	MUX(0, 0, 1, PULL_OFF))
#define PIN_FN_GPIO_PORT101	_PIN_FN(PIN_FCE0_N,	MUX(0, 0, 1, PULL_OFF))
#define PIN_FN_GPIO_PORT106	_PIN_FN(PIN_IOIS16_N,	MUX(0, 0, 1, PULL_OFF))
#define PIN_FN_GPIO_PORT107	_PIN_FN(PIN_WE3_N,	MUX(0, 1, 0, PULL_OFF))
#define PIN_FN_GPIO_PORT108	_PIN_FN(PIN_WE2_N,	MUX(0, 1, 0, PULL_OFF))
#define PIN_FN_GPIO_PORT111	_PIN_FN(PIN_CS4_N,	MUX(0, 0, 1, PULL_OFF))
#define PIN_FN_GPIO_PORT114	_PIN_FN(PIN_RDWR,	MUX(0, 0, 1, PULL_OFF))
#define PIN_FN_GPIO_PORT166	_PIN_FN(PIN_D21,	MUX(0, 1, 0, PULL_OFF))
#define PIN_FN_GPIO_PORT208	_PIN_FN(PIN_SDHID3_2,	MUX(0, 1, 0, PULL_OFF))

#define PIN_FN_SCIFA2_TXD_201	_PIN_FN(PIN_MEMC_WAIT,	MUX(2, 1, 0, PULL_OFF))
#define PIN_FN_SCIFA2_RXD_200	_PIN_FN(PIN_MEMC_NOE,	MUX(2, 0, 1, PULL_UP))
#define PIN_FN_SCIFA2_RTS_96	_PIN_FN(PIN_MEMC_ADV,	MUX(2, 1, 0, PULL_DOWN))
#define PIN_FN_SCIFA2_CTS_95	_PIN_FN(PIN_MEMC_CS1,	MUX(2, 0, 1, PULL_UP))

#define PIN_FN_SDHI0_CLK_82	_PIN_FN(PIN_SDHICLK_0,	MUX(1, 1, 0, PULL_OFF))
#define PIN_FN_SDHI0_CMD_76	_PIN_FN(PIN_SDHICMD_0,	MUX(1, 1, 1, PULL_OFF))
#define PIN_FN_SDHI0_D0_77	_PIN_FN(PIN_SDHID0_0,	MUX(1, 1, 1, PULL_OFF))
#define PIN_FN_SDHI0_D1_78	_PIN_FN(PIN_SDHID1_0,	MUX(1, 1, 1, PULL_OFF))
#define PIN_FN_SDHI0_D2_79	_PIN_FN(PIN_SDHID2_0,	MUX(1, 1, 1, PULL_OFF))
#define PIN_FN_SDHI0_D3_80	_PIN_FN(PIN_SDHID3_0,	MUX(1, 1, 1, PULL_OFF))
#define PIN_FN_SDHI0_CD_81	_PIN_FN(PIN_SDHICD_0,	MUX(1, 0, 1, PULL_OFF))
#define PIN_FN_SDHI0_WP_82	_PIN_FN(PIN_SDHIWP_0,	MUX(1, 0, 1, PULL_OFF))

#define PIN_FN_ETH_CRS_205	_PIN_FN(PIN_SDHID0_2,	MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_MDC_206	_PIN_FN(PIN_SDHID1_2,	MUX(3, 1, 0, PULL_OFF))
#define PIN_FN_ETH_MDIO_207	_PIN_FN(PIN_SDHID2_2,	MUX(3, 1, 1, PULL_OFF))
#define PIN_FN_ETH_TX_ER_203	_PIN_FN(PIN_SDHICLK_2,	MUX(3, 1, 0, PULL_OFF))
#define PIN_FN_ETH_RX_ER_204	_PIN_FN(PIN_SDHICMD_2,	MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_ERXD0_185	_PIN_FN(PIN_SCIFA_SCK_1,MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_ERXD1_186	_PIN_FN(PIN_SCIFB_RTS_N,MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_ERXD2_187	_PIN_FN(PIN_SCIFB_CTS_N,MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_ERXD3_188	_PIN_FN(PIN_SCIFA_SCK_0,MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_TX_CLK_184	_PIN_FN(PIN_DACK_1_N,	MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_TX_EN_183	_PIN_FN(PIN_DREQ_1_N,	MUX(3, 1, 0, PULL_OFF))
#define PIN_FN_ETH_ETXD0_171	_PIN_FN(PIN_D16,	MUX(3, 1, 0, PULL_OFF))
#define PIN_FN_ETH_ETXD1_170	_PIN_FN(PIN_D17,	MUX(3, 1, 0, PULL_OFF))
#define PIN_FN_ETH_ETXD2_169	_PIN_FN(PIN_D18,	MUX(3, 1, 0, PULL_OFF))
#define PIN_FN_ETH_ETXD3_168	_PIN_FN(PIN_D19,	MUX(3, 1, 0, PULL_OFF))
#define PIN_FN_ETH_COL_163	_PIN_FN(PIN_D26,	MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_RX_DV_161	_PIN_FN(PIN_D28,	MUX(3, 0, 1, PULL_OFF))
#define PIN_FN_ETH_RX_CLK_174	_PIN_FN(PIN_A26,	MUX(3, 0, 1, PULL_OFF))

/* for platform */
#define xBSPIF0			PIN_FN_GPIO_PORT111
#define xBSPIF1			PIN_FN_GPIO_PORT17
#define xBSPIF2			PIN_FN_GPIO_PORT16
#define xJP1			PIN_FN_GPIO_PORT101
#define xLED1			PIN_FN_GPIO_PORT107
#define xLED2			PIN_FN_GPIO_PORT108
#define xLED3			PIN_INVALID
#define xLED4			PIN_INVALID
#define xEEPROM_SCL		PIN_FN_GPIO_PORT90
#define xEEPROM_SDA		PIN_FN_GPIO_PORT89
#define xDBGMD_EN_N		PIN_FN_GPIO_PORT18
#define xUART_TXD		PIN_FN_SCIFA2_TXD_201
#define xUART_RXD		PIN_FN_SCIFA2_RXD_200
#define xUART_RTS		PIN_FN_SCIFA2_RTS_96
#define xUART_CTS		PIN_FN_SCIFA2_CTS_95

/* for driver */
#define xSDHI0_VS_B		PIN_INVALID
#define xSDHI0_PON		PIN_INVALID
#define xSDSLOT1_PON		PIN_FN_GPIO_PORT166
#define xSDSLOT1_CD		PIN_INVALID

#define xMMC0_RST_B		PIN_INVALID

#define xETH_PHYRST		PIN_FN_GPIO_PORT208

#endif