#ifndef _HERMIT_TARGET_PLATFORM_BOARD_DEV_H_
#define _HERMIT_TARGET_PLATFORM_BOARD_DEV_H_

#include <io.h>
#include <arch/ioregs.h>

#define MACH_NUMBER (MACH_ARMADILLO500)

static inline void
init_mux_private(void)
{
	/* Jumper */
	mxc_set_mux(MUX_PIN(xJP1), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(xJP2), MUX_O_GPIO | MUX_I_GPIO);

	/* LED & BASE ID */
	mxc_set_mux(MUX_PIN(xLED1), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(xLED2), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(xLED3), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(xLED4), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(xLED5), MUX_O_GPIO | MUX_I_GPIO);

	/* UART1 */
	mxc_set_mux(MUX_PIN(CTS1), MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(RTS1), MUX_O_FUNC | MUX_I_FUNC);

	/* UART2 */
	mxc_set_mux(MUX_PIN(RXD2), MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(TXD2), MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(CTS2), MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(RTS2), MUX_O_FUNC | MUX_I_FUNC);

	/* CF */
	mxc_set_mux(MUX_PIN(xCF_PWREN), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_gpio(GPIO_PIN(xCF_PWREN), GPIO_OUTPUT, GPIO_HIGH);/* OFF */

	/* LCD */
	mxc_set_mux(MUX_PIN(CONTRAST), MUX_O_FUNC | MUX_I_FUNC);
}

static inline void
init_weim_private(void)
{
	/* CS3: smc911x */
	write32(WEIM_BASE_ADDR + CSCR3U, 0x00004803);
	write32(WEIM_BASE_ADDR + CSCR3L, 0x24002531);
	write32(WEIM_BASE_ADDR + CSCR3A, 0x00240000);
}

static inline void
init_ipu(void)
{
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

	/* BASE Board ID */
	mxc_set_gpio(GPIO_PIN(xID1), GPIO_INPUT, 0);
	mxc_set_gpio(GPIO_PIN(xID2), GPIO_INPUT, 0);
	mxc_set_gpio(GPIO_PIN(xID3), GPIO_INPUT, 0);
	mxc_set_gpio(GPIO_PIN(xID4), GPIO_INPUT, 0);

	info->base_board_id = mxc_get_gpio(GPIO_PIN(xID1)) ? (1 << 0):0;
	info->base_board_id |= mxc_get_gpio(GPIO_PIN(xID2)) ? (1 << 1):0;
	info->base_board_id |= mxc_get_gpio(GPIO_PIN(xID3)) ? (1 << 2):0;
	info->base_board_id |= mxc_get_gpio(GPIO_PIN(xID4)) ? (1 << 3):0;

	mxc_set_gpio(GPIO_PIN(xID1), GPIO_OUTPUT, 0);
	mxc_set_gpio(GPIO_PIN(xID2), GPIO_OUTPUT, 0);
	mxc_set_gpio(GPIO_PIN(xID3), GPIO_OUTPUT, 0);
	mxc_set_gpio(GPIO_PIN(xID4), GPIO_OUTPUT, 0);
}

#define PRIVATE_LED_ON()	mxc_set_gpio(GPIO_PIN(xLED5), 1, 1)
#define PRIVATE_LED_OFF()	mxc_set_gpio(GPIO_PIN(xLED5), 1, 0)

#endif /* _HERMIT_TARGET_PLATFORM_BOARD_DEV_H_ */
