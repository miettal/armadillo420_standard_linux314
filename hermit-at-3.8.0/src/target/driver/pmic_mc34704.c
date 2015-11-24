#include <autoconf.h>
#include <platform.h>
#include <herrno.h>
#include <i2c.h>
#include <i2c_mx25.h>

static struct i2c_device pmic_dev = {
	.port = 1,
	.devid = 0xa8,
	.wait = 5,
};

int mc34704_read(u8 addr, u8 *val)
{
	u8 sub[1] = { addr | 0x80 };
	struct i2c_msg msgs[] = {
		[0] = { sub, 1, I2C_M_WR },
		[1] = { val, 1, I2C_M_RD | I2C_REPSTART },
	};

	return mx25_i2c_xfer(&pmic_dev, msgs, 2);
}

int mc34704_write(u8 addr, u8 val)
{
	u8 sub[2] = { addr };
	struct i2c_msg msgs[] = {
		[0] = { sub, 2, I2C_M_WR },
	};
	sub[1] = val;
	return mx25_i2c_xfer(&pmic_dev, msgs, 1);
}
