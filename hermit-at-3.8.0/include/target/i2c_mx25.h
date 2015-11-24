#ifndef _HERMIT_TARGET_I2C_MX25_H_
#define _HERMIT_TARGET_I2C_MX25_H_

extern int mx25_i2c_xfer(struct i2c_device *dev, struct i2c_msg *msgs,
			 size_t num);
extern void mx25_setup_i2c(int port);

#endif
