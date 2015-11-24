#ifndef _HERMIT_TARGET_I2C_MX25_GPIO_H_
#define _HERMIT_TARGET_I2C_MX25_GPIO_H_

#include <htypes.h>
#include <i2c.h>

#define I2C_WR_BIT 0
#define I2C_RD_BIT 1

struct mx25_gpio_i2c_device {
	int scl_pin;
	int sda_pin;
	u32 wait_usecs;
};

extern int mx25_i2c_gpio_bus_force_idle(struct mx25_gpio_i2c_device *dev);
extern int mx25_i2c_gpio_xfer(struct mx25_gpio_i2c_device *dev, struct i2c_msg *msgs, size_t num);

#endif
