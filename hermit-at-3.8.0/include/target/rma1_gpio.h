#ifndef _HERMIT_TARGET_RMA1_GPIO_H_
#define _HERMIT_TARGET_RMA1_GPIO_H_

#include <htypes.h>

extern int gpio_direction_input(u32 pin);
extern int gpio_direction_output(u32 pin, u8 level);
extern int gpio_get_value(u32 pin);
extern int gpio_set_value(u32 pin, u8 level);

#endif
