#include <io.h>
#include <hermit.h>
#include <herrno.h>
#include <platform.h>
#include <delay.h>
#include <string.h>
#include <arch/iomux.h>
#include <i2c_mx25_gpio.h>

static inline u8 i2c_getscl(struct mx25_gpio_i2c_device *dev)
{
	mx25_gpio_input(dev->scl_pin);
	return mx25_gpio_get_psr(dev->scl_pin);
}

static inline void i2c_setscl(struct mx25_gpio_i2c_device *dev, u8 level)
{
	if (level)
		mx25_gpio_input(dev->scl_pin);
	else
		mx25_gpio_output(dev->scl_pin, 0);
}

static inline u8 i2c_getsda(struct mx25_gpio_i2c_device *dev)
{
	mx25_gpio_input(dev->sda_pin);
	return mx25_gpio_get_psr(dev->sda_pin);
}

static inline void i2c_setsda(struct mx25_gpio_i2c_device *dev, u8 level)
{
	if (level)
		mx25_gpio_input(dev->sda_pin);
	else
		mx25_gpio_output(dev->sda_pin, 0);
}

static inline u8 i2c_sda(struct mx25_gpio_i2c_device *dev)
{
	return i2c_getsda(dev);
}

static inline void i2c_sdalo(struct mx25_gpio_i2c_device *dev)
{
	i2c_setsda(dev, 0);
	udelay((dev->wait_usecs + 1) / 2);
}

static inline void i2c_sdahi(struct mx25_gpio_i2c_device *dev)
{
	i2c_setsda(dev, 1);
	udelay((dev->wait_usecs + 1) / 2);
}

static inline void i2c_scllo(struct mx25_gpio_i2c_device *dev)
{
	i2c_setscl(dev, 0);
	udelay(dev->wait_usecs / 2);
}

static inline void i2c_sclhi(struct mx25_gpio_i2c_device *dev)
{
	i2c_setscl(dev, 1);
	udelay(dev->wait_usecs);
}

static void i2c_acknak(struct mx25_gpio_i2c_device *dev, int is_ack)
{
	if (is_ack)
		i2c_setsda(dev, 0);
	udelay((dev->wait_usecs + 1) / 2);
	i2c_sclhi(dev);
	i2c_scllo(dev);
}

static void i2c_start(struct mx25_gpio_i2c_device *dev)
{
	i2c_setsda(dev, 0);
	udelay(dev->wait_usecs);
	i2c_scllo(dev);
}

static void i2c_repstart(struct mx25_gpio_i2c_device *dev)
{
	i2c_sdahi(dev);
	i2c_sclhi(dev);
	i2c_setsda(dev, 0);
	udelay(dev->wait_usecs);
	i2c_scllo(dev);
}

static void i2c_stop(struct mx25_gpio_i2c_device *dev)
{
	i2c_sdalo(dev);
	i2c_sclhi(dev);
	i2c_setsda(dev, 1);
	udelay(dev->wait_usecs);
}

static inline void i2c_init(struct mx25_gpio_i2c_device *dev)
{
	i2c_stop(dev);
}

static int i2c_outb(struct mx25_gpio_i2c_device *dev, u8 c)
{
	u8 bit;
	int i;
	int ack;

	for (i = 7; i >= 0; i--) {
		bit = ((c >> i) & 1);
		i2c_setsda(dev, bit);
		udelay((dev->wait_usecs + 1) / 2);

		i2c_sclhi(dev);
		i2c_scllo(dev);
	}

	i2c_sdahi(dev);
	i2c_sclhi(dev);
	ack = !i2c_sda(dev);
	i2c_scllo(dev);

	return !ack;
}

static u8 i2c_inb(struct mx25_gpio_i2c_device *dev)
{
	u8 indata = 0;
	int i;

	i2c_sdahi(dev);
	for (i = 0; i < 8; i++) {
		i2c_sclhi(dev);

		indata <<= 1;
		if (i2c_sda(dev))
			indata |= 0x01;

		i2c_setscl(dev, 0);
		udelay((i == 7) ? (dev->wait_usecs / 2) : dev->wait_usecs);
	}

	return indata;
}

static void i2c_readbytes(struct mx25_gpio_i2c_device *dev, struct i2c_msg *msg)
{
	u8 *temp = msg->buf;
	int count = msg->len ? msg->len : 256;

	while (count > 0) {
		*temp = i2c_inb(dev);

		count--;
		temp++;

		i2c_acknak(dev, count);
	}
}

static int i2c_sendbytes(struct mx25_gpio_i2c_device *dev, struct i2c_msg *msg)
{
	const u8 *temp = msg->buf;
	int count = msg->len ? msg->len : 256;
	int repstart = 0;
	int ret;

	while (count > 0) {
		ret = i2c_outb(dev, *temp);

		if (!ret) {
			count--;
			temp++;
		} else {
			if ((count == msg->len) && !repstart) {
				i2c_repstart(dev);
				repstart = 1;
				continue;
			}
			return -H_EIO;
		}
	}

	return 0;
}

int mx25_i2c_gpio_bus_force_idle(struct mx25_gpio_i2c_device *dev)
{
	int i;

	mx25_gpio_input(dev->sda_pin);

	for (i = 0; i < 9; i++) {
		i2c_scllo(dev);
		i2c_sclhi(dev);
	}

	return 0;
}

int mx25_i2c_gpio_xfer(struct mx25_gpio_i2c_device *dev, struct i2c_msg *msgs, size_t num)
{
	int ret = 0;
	int i;

	i2c_start(dev);
	for (i = 0; i < num; i++) {
		if (msgs[i].flags & I2C_REPSTART)
			i2c_repstart(dev);

		if (msgs[i].flags & I2C_M_RD) {
			i2c_readbytes(dev, &msgs[i]);
		} else {
			ret = i2c_sendbytes(dev, &msgs[i]);
			if (ret)
				goto xfer_end;
		}
	}

xfer_end:
	i2c_stop(dev);
	return ret;
}
