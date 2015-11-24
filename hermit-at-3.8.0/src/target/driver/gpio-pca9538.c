#include <autoconf.h>
#include <platform.h>
#include <herrno.h>
#include <arch/iomux.h>
#include <i2c_mx25_gpio.h>

struct mx25_gpio_i2c_device i2c_pca9538 = {
	.scl_pin = MX25_PIN_CSPI1_SCLK,
	.sda_pin = MX25_PIN_CSPI1_SS1,
	.wait_usecs = 6,
};

#define PCA953X_INPUT		(0)
#define PCA953X_OUTPUT		(1)
#define PCA953X_INVERT		(2)
#define PCA953X_DIRECTION	(3)
#define PCA953X_START_SLAVE_ADDR	(0x70)

static u8 pca9538_slave_addr = PCA953X_START_SLAVE_ADDR;
static u8 pca9538_scan_slave_addr[] = {
	PCA953X_START_SLAVE_ADDR,
	PCA953X_START_SLAVE_ADDR + 1,
	PCA953X_START_SLAVE_ADDR + 2,
	PCA953X_START_SLAVE_ADDR + 3,
};

static int pca9538_slave_addr_is_vaild(u8 slave_addr)
{
	u8 head_write = slave_addr << 1 | I2C_WR_BIT;
	u8 head_read =  slave_addr << 1 | I2C_RD_BIT;
	u8 val;
	u8 reg = PCA953X_INPUT;

	struct i2c_msg msgs[] = {
		[0] = { &head_write, 1, I2C_M_WR },
		[1] = { &reg, 1, I2C_M_WR },
		[2] = { &head_read, 1, I2C_M_WR | I2C_REPSTART},
		[3] = { &val, 1, I2C_M_RD },
	};

	return !mx25_i2c_gpio_xfer(&i2c_pca9538, msgs, ARRAY_SIZE(msgs));
}

static int pca9538_write_reg(u8 reg, u8 val)
{
	u8 head_write = pca9538_slave_addr << 1 | I2C_WR_BIT;

	struct i2c_msg msgs[] = {
		[0] = { &head_write, 1, I2C_M_WR },
		[1] = { &reg, 1, I2C_M_WR },
		[2] = { &val, 1, I2C_M_WR },
	};

	return mx25_i2c_gpio_xfer(&i2c_pca9538, msgs, ARRAY_SIZE(msgs));
}

static int pca9538_read_reg(u8 reg, u8 *val)
{
	u8 head_write = pca9538_slave_addr << 1 | I2C_WR_BIT;
	u8 head_read =  pca9538_slave_addr << 1 | I2C_RD_BIT;

	struct i2c_msg msgs[] = {
		[0] = { &head_write, 1, I2C_M_WR },
		[1] = { &reg, 1, I2C_M_WR },
		[2] = { &head_read, 1, I2C_M_WR | I2C_REPSTART},
		[3] = { val, 1, I2C_M_RD },
	};

	return mx25_i2c_gpio_xfer(&i2c_pca9538, msgs, ARRAY_SIZE(msgs));
}

void pca9538_detect(void)
{
	int i;

	mx25_i2c_gpio_bus_force_idle(&i2c_pca9538);

	for (i = 0; i < ARRAY_SIZE(pca9538_scan_slave_addr); i++) {
		if (pca9538_slave_addr_is_vaild(pca9538_scan_slave_addr[i])) {
			pca9538_slave_addr = pca9538_scan_slave_addr[i];
			break;
		}
	}
}

u8 pca9538_get_slave_addr(void)
{
	return pca9538_slave_addr;
}

int pca9538_gpio_direction_input(unsigned offset)
{
	u8 reg;
	int ret;

	ret = pca9538_read_reg(PCA953X_DIRECTION, &reg);
	if (ret)
		return ret;

	reg |= (1u << offset);
	return pca9538_write_reg(PCA953X_DIRECTION, reg);
}

int pca9538_gpio_direction_output(unsigned offset, int val)
{
	u8 reg;
	int ret;

	ret = pca9538_read_reg(PCA953X_OUTPUT, &reg);
	if (ret)
		return ret;

	if (val)
		reg |= (1u << offset);
	else
		reg &= ~(1u << offset);

	ret = pca9538_write_reg(PCA953X_OUTPUT, reg);
	if (ret)
		return ret;


	ret = pca9538_read_reg(PCA953X_DIRECTION, &reg);
	if (ret)
		return ret;

	reg &= ~(1u << offset);
	return pca9538_write_reg(PCA953X_DIRECTION, reg);
}

int pca9538_gpio_get_value(unsigned offset)
{
	u8 reg;
	int ret;

	ret = pca9538_read_reg(PCA953X_INPUT, &reg);
	if (ret)
		return ret;

	return (reg & (1u << offset)) ? 1 : 0;
}
