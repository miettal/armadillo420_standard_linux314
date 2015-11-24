#ifndef _HERMIT_TARGET_PLATFORM_BOARD_ZERO_H_
#define _HERMIT_TARGET_PLATFORM_BOARD_ZERO_H_

#define MACH_NUMBER (MACH_ARMADILLO500)

static inline void
init_mux_private(void)
{
}

static inline void
init_weim_private(void)
{
}

static inline void
init_ipu(void)
{
}

static inline void
init_i2c(void)
{
}

static inline void
get_board_info_private(struct board_private *info)
{
	info->jumper1 = 1;
	info->jumper2 = 1;
}

#define PRIVATE_LED_ON()
#define PRIVATE_LED_OFF()

#endif /* _HERMIT_TARGET_PLATFORM_BOARD_ZERO_H_ */
