#ifndef _HERMIT_TARGET_LED_H_
#define _HERMIT_TARGET_LED_H_

#include <htypes.h>
#include <hermit.h>

#define LED_RED		(BIT(0))
#define LED_GREEN	(BIT(1))
#define LED_YELLOW	(BIT(2))

extern void led_on(u32 leds);
extern void led_off(u32 leds);

#endif
