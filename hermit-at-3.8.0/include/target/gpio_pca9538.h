#ifndef _HERMIT_TARGET_GPIO_PCA9538_H_
#define _HERMIT_TARGET_GPIO_PCA9538_H_

#include <htypes.h>

extern void pca9538_detect(void);
extern u8 pca9538_get_slave_addr(void);
extern int pca9538_gpio_direction_input(unsigned offset);
extern int pca9538_gpio_direction_output(unsigned offset, int reg);
extern int pca9538_gpio_get_value(unsigned offset);

#endif
