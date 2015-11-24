#ifndef _HERMIT_TARGET_PLATFORM_BOARD_FX_H_
#define _HERMIT_TARGET_PLATFORM_BOARD_FX_H_

#include <io.h>
#include <arch/ioregs.h>

#define MACH_NUMBER (MACH_ARMADILLO500FX)

static inline void
init_mux_private(void)
{
	/* Jumper */
	mxc_set_mux(MUX_PIN(xJP1), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(xJP2), MUX_O_GPIO | MUX_I_GPIO);

	/* LED & BASE ID */
	mxc_set_mux(MUX_PIN(xLED_STATUS), MUX_O_GPIO | MUX_I_GPIO);

	/* LCD */
	mxc_set_mux(MUX_PIN(CONTRAST), MUX_O_FUNC | MUX_I_FUNC);

	/* GPIO */
#define P_CSI_GPIO (PAD_CTL_100K_PU | PAD_CTL_PUE_PUD | PAD_CTL_PKE_ENABLE)
	mxc_set_pad(PAD_PIN(CSI_D4), P_CSI_GPIO);
	mxc_set_pad(PAD_PIN(CSI_D5), P_CSI_GPIO);
#undef P_CSI_GPIO
}

static inline void
init_weim_private(void)
{
	/* CS3: smc921x */
	write32(WEIM_BASE_ADDR + CSCR3U, 0x00001800);
	write32(WEIM_BASE_ADDR + CSCR3L, 0xff00f5f1);
	write32(WEIM_BASE_ADDR + CSCR3A, 0x00ff0000);
}

static inline void
init_ipu(void)
{
	/* Set brightness (contrast) to zero */
	write32(IPU_BASE_ADDR + SDC_PWM_CTRL, 0x0);
}

static inline void
init_i2c(void)
{
	int i;
	u32 old_mux;

	old_mux = mxc_get_mux(MUX_PIN(CSPI2_MOSI));

	mxc_set_mux(MUX_PIN(CSPI2_MOSI), MUX_O_FUNC | MUX_I_FUNC);

	write32(CSPI2_BASE_ADDR + CONREG, 0x00071f0b);

	for (i=0; i<8; i++) {
		/* wait TxFIFO empty */
		while (!(read32(CSPI2_BASE_ADDR + STATREG) & 0x0001));

		write32(CSPI2_BASE_ADDR + TXDATA, 0x55555555);
	}

	/* wait busy */
	while (!(read32(CSPI2_BASE_ADDR + STATREG) & 0x0100));
	write32(CSPI2_BASE_ADDR + STATREG, 0x0100);

	write32(CSPI2_BASE_ADDR + CONREG, 0x0);

	mxc_set_mux(MUX_PIN(CSPI2_MOSI), old_mux);
}

static inline void
get_board_info_private(struct board_private *info)
{
	/* Jumper */
	mxc_set_gpio(GPIO_PIN(xJP1), GPIO_INPUT, 0);
	mxc_set_gpio(GPIO_PIN(xJP2), GPIO_INPUT, 0);

	info->jumper1 = mxc_get_gpio(GPIO_PIN(xJP1)) ? 0 : 1;
	info->jumper2 = mxc_get_gpio(GPIO_PIN(xJP2)) ? 0 : 1;
}

#define PRIVATE_LED_ON()	mxc_set_gpio(GPIO_PIN(xLED_STATUS), 1, 1)
#define PRIVATE_LED_OFF()	mxc_set_gpio(GPIO_PIN(xLED_STATUS), 1, 0)

#endif /* _HERMIT_TARGET_PLATFORM_BOARD_FX_H_ */
