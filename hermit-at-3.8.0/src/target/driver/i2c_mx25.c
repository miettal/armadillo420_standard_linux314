#include <arch/system.h>
#include <herrno.h>
#include <htypes.h>
#include <io.h>
#include <i2c.h>
#include <arch/iomux.h>
#include <arch/clock.h>
#include <delay.h>

#define I2C1_ENABLE 1
#define I2C2_ENABLE 0
#define I2C3_ENABLE 0

extern struct iomux_info i2c1_pins[];

struct i2c_port {
	addr_t base;
	struct iomux_info *iomux;
	u32 clock_num;
};

struct i2c_port i2c[4] = {
	[0] = { 0, 0, 0 },
	[1] = { I2C1_BASE_ADDR, i2c1_pins, 0 },
};

static int mx25_i2c_wait_for_xfer(struct i2c_device *dev, struct i2c_msg *msg)
{
	struct i2c_port *port = &i2c[dev->port];
	int timeout = 1000;
	u16 val;
	while (timeout--) {
		val = read16(port->base + I2C_I2SR);
		if (val & 0x02) {
			write16(port->base + I2C_I2SR, 0);
			return 0;
		}
		udelay(1);
	}
	return -H_EIO;
}

static int mx25_i2c_start(struct i2c_device *dev, struct i2c_msg *msg)
{
	struct i2c_port *port = &i2c[dev->port];
	u16 val;
	val = read16(port->base + I2C_I2CR);
	val |= 0x20;
	write16(port->base + I2C_I2CR, val);

	while (!(read16(port->base + I2C_I2SR) & 0x20));

	val = read16(port->base + I2C_I2CR);
	val |= 0x10;
	write16(port->base + I2C_I2CR, val);

	write16(port->base + I2C_I2DR,
		dev->devid | (msg->flags & I2C_M_RD ? 1 : 0));
	return mx25_i2c_wait_for_xfer(dev, msg);
}

static int mx25_i2c_repstart(struct i2c_device *dev, struct i2c_msg *msg)
{
	struct i2c_port *port = &i2c[dev->port];
	u16 val;
	int ret;
	val = read16(port->base + I2C_I2CR);
	val |= 0x04;
	write16(port->base + I2C_I2CR, val);

	write16(port->base + I2C_I2DR,
		dev->devid | (msg->flags & I2C_M_RD ? 1 : 0));
	ret = mx25_i2c_wait_for_xfer(dev, msg);

	val = read16(port->base + I2C_I2CR);
	val &= ~0x04;
	write16(port->base + I2C_I2CR, val);

	return ret;
}

static void mx25_i2c_stop(struct i2c_device *dev)
{
	struct i2c_port *port = &i2c[dev->port];
	u16 val;
	val = read16(port->base + I2C_I2CR);
	val &= 0xc0;
	write16(port->base + I2C_I2CR, val);

	while (read16(port->base + I2C_I2SR) & 0x20);
}

static int mx25_i2c_readb(struct i2c_device *dev, struct i2c_msg *msg)
{
	struct i2c_port *port = &i2c[dev->port];
	u16 val;
	int ret;
	int i;
	val = read16(port->base + I2C_I2CR);
	val &= ~0x10;
	write16(port->base + I2C_I2CR, val);

	val = read16(port->base + I2C_I2DR) & 0xff;
	ret = mx25_i2c_wait_for_xfer(dev, msg);
	if (ret)
		return ret;

	for (i=0; i<msg->len; i++) {
		if (i == msg->len - 1) {
			val = read16(port->base + I2C_I2CR);
			val |= 0x08;
			write16(port->base + I2C_I2CR, val);
		}
		msg->buf[i] = read16(port->base + I2C_I2DR) & 0xff;
		ret = mx25_i2c_wait_for_xfer(dev, msg);
		if (ret)
			return ret;
	}
	return 0;
}

static int mx25_i2c_writeb(struct i2c_device *dev, struct i2c_msg *msg)
{
	struct i2c_port *port = &i2c[dev->port];
	u16 val;
	int ret;
	int i;
	val = read16(port->base + I2C_I2CR);
	val |= 0x10;
	write16(port->base + I2C_I2CR, val);

	for (i=0; i<msg->len; i++) {
		write16(port->base + I2C_I2DR, msg->buf[i]);
		ret = mx25_i2c_wait_for_xfer(dev, msg);
		if (ret)
			return ret;
	}
	return 0;
}

int mx25_i2c_xfer(struct i2c_device *dev, struct i2c_msg *msgs, size_t num)
{
	struct i2c_port *port = &i2c[dev->port];
	int ret;
	int i;

	if (num == 0)
		return 0;

	if (read16(port->base + I2C_I2SR) & 0x20)
		return -H_EIO;

	for (i=0; i<num; i++) {
		if (i == 0) {
			ret = mx25_i2c_start(dev, &msgs[i]);
			if (ret)
				goto xfer_end;
		} else {
			if (msgs[i].flags & I2C_REPSTART) {
				ret = mx25_i2c_repstart(dev, &msgs[i]);
				if (ret)
					goto xfer_end;
			}
		}
		if (msgs[i].flags & I2C_M_RD) {
			ret = mx25_i2c_readb(dev, &msgs[i]);
			if (ret)
				goto xfer_end;
		} else {
			ret = mx25_i2c_writeb(dev, &msgs[i]);
			if (ret)
				goto xfer_end;
		}
	}
xfer_end:
	mx25_i2c_stop(dev);
	mdelay(dev->wait);
	return ret;
}

void mx25_setup_i2c(int port)
{
	int i;

	/* I2C bus initialize using SCL(GPIO) */
	mx25_iomux_config(i2c[port].iomux, 0);
	mx25_gpio_input(i2c1_pins[1].pin);
	if (mx25_gpio_get_psr(i2c1_pins[1].pin) == 0) {
		/* I2C Bus Busy... */
		for (i=0; i<64; i++) {
			mx25_gpio_output(i2c1_pins[0].pin, 0);
			udelay(100);
			mx25_gpio_output(i2c1_pins[0].pin, 1);
			udelay(100);
		}
	}

	mx25_iomux_config(i2c[port].iomux, 1);
	mx25_per_clock_enable(6);

	write16(i2c[port].base + I2C_I2CR, 0x0);
	write16(i2c[port].base + I2C_IFDR, 0x1d); /* divid by 2560 */
	write16(i2c[port].base + I2C_I2SR, 0x0);
	write16(i2c[port].base + I2C_I2CR, 0x80);
}
