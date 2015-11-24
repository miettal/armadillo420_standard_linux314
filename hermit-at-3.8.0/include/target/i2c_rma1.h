#ifndef _HERMIT_TARGET_I2C_RMA1_H_
#define _HERMIT_TARGET_I2C_RMA1_H_

extern int rma1_i2c_xfer(struct i2c_device *dev, struct i2c_msg *msgs,
			 size_t num);
extern void rma1_setup_i2c(int port);

#endif
