#ifndef _HERMIT_TARGET_I2C_H_
#define _HERMIT_TARGET_I2C_H_

#include <htypes.h>

struct i2c_device {
	int port;
	u8 devid;
	unsigned long wait; /* milliseconds */
};

struct i2c_msg {
	u8 *buf;
	size_t len;
	u8 flags;
#define I2C_M_WR 0
#define I2C_M_RD 1
#define I2C_REPSTART 2
};

#endif
