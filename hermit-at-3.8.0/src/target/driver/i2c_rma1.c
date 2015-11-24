#define CORE_NAME "i2c_rma1"

#include <hermit.h>
#include <arch/system.h>
#include <herrno.h>
#include <htypes.h>
#include <io.h>
#include <i2c.h>
#include <delay.h>

static addr_t i2c_base(int port)
{
	if (port == 1)
		return I2C0_BASE_ADDR;
	if (port == 2)
		return I2C1_BASE_ADDR;
	return 0;
}

static int rma1_i2c_wait_for_xfer(struct i2c_device *dev, struct i2c_msg *msg)
{
	addr_t base = i2c_base(dev->port);
	int retry = 100000;
	u8 stat;

	trace();

	while (1) {
		stat = read8(base + I2C_ICSR);
		dev_vdbg("icsr = %p\n", stat);
		if (stat & 0x2) {
			write8(base + I2C_ICSR, stat & ~0x2);
			continue;
		}
		if (stat & 0x1)
			break;
		if (retry-- == 0)
			return -H_EIO;
		udelay(1);
	}
	write8(base + I2C_ICSR, stat & 0x1);

	return 0;
}


static int rma1_i2c_start(struct i2c_device *dev, struct i2c_msg *msg)
{
	addr_t base = i2c_base(dev->port);

	trace();

	write8(base + I2C_ICCR, 0x81);
	write8(base + I2C_ICCL, 0xff);
	write8(base + I2C_ICCH, 0xff);

	write8(base + I2C_ICIC, 0x1);
	write8(base + I2C_ICCR, 0x84 | (msg->flags & I2C_M_RD ? 0:0x10));
	rma1_i2c_wait_for_xfer(dev, msg);

	write8(base + I2C_ICDR,
	       dev->devid | (msg->flags & I2C_M_RD ? 1 : 0));
	rma1_i2c_wait_for_xfer(dev, msg);

	return 0;
}

static int rma1_i2c_repstart(struct i2c_device *dev, struct i2c_msg *msg)
{
	addr_t base = i2c_base(dev->port);

	trace();

	write8(base + I2C_ICCR, 0x84 | (msg->flags & I2C_M_RD ? 0:0x10));
	rma1_i2c_wait_for_xfer(dev, msg);

	write8(base + I2C_ICDR,
	       dev->devid | (msg->flags & I2C_M_RD ? 1 : 0));
	rma1_i2c_wait_for_xfer(dev, msg);

	return 0;
}

static void rma1_i2c_stop(struct i2c_device *dev)
{
	addr_t base = i2c_base(dev->port);

	write8(base + I2C_ICIC, 0x00);
	write8(base + I2C_ICSR, 0x00);
	write8(base + I2C_ICCR, 0x90);
	mdelay(1);
	write8(base + I2C_ICCR, 0x00);
}

static int rma1_i2c_readb(struct i2c_device *dev, struct i2c_msg *msg)
{
	addr_t base = i2c_base(dev->port);
	int ret;
	int i;

	trace();

	for (i=0; i<msg->len; i++) {
		msg->buf[i] = read8(base + I2C_ICDR);
		ret = rma1_i2c_wait_for_xfer(dev, msg);
		if (ret)
			return ret;
	}

	return 0;
}

static int rma1_i2c_writeb(struct i2c_device *dev, struct i2c_msg *msg)
{
	addr_t base = i2c_base(dev->port);
	int ret;
	int i;

	trace();

	for (i=0; i<msg->len; i++) {
		write8(base + I2C_ICDR, msg->buf[i]);
		ret = rma1_i2c_wait_for_xfer(dev, msg);
		if (ret)
			return ret;
	}

	return 0;
}

int rma1_i2c_xfer(struct i2c_device *dev, struct i2c_msg *msgs, size_t num)
{
	int ret;
	int i;

	trace();

	if (num == 0)
		return 0;

	for (i=0; i<num; i++) {
		if (i == 0) {
			ret = rma1_i2c_start(dev, &msgs[i]);
			if (ret)
				goto xfer_end;
		} else {
			if (msgs[i].flags & I2C_REPSTART) {
				ret = rma1_i2c_repstart(dev, &msgs[i]);
				if (ret)
					goto xfer_end;
			}
		}
		if (msgs[i].flags & I2C_M_RD) {
			ret = rma1_i2c_readb(dev, &msgs[i]);
			if (ret)
				goto xfer_end;
		} else {
			ret = rma1_i2c_writeb(dev, &msgs[i]);
			if (ret)
				goto xfer_end;
		}
	}
xfer_end:
	rma1_i2c_stop(dev);
	mdelay(dev->wait);
	return ret;
}

void rma1_setup_i2c(int port)
{
	addr_t i2c1_base = i2c_base(1);
	addr_t i2c2_base = i2c_base(2);
	addr_t base = i2c_base(port);

	trace();

	cpg_clr_mstpcr(116);
	cpg_clr_mstpcr(323);

	/* Initialize for errata */
	write8(i2c2_base + I2C_ICCR, 0x81);
	write8(i2c2_base + I2C_ICSTART, 0x10);
	udelay(10);
	write8(i2c2_base + I2C_ICCR, 0x01);
	write8(i2c2_base + I2C_ICSTART, 0x00);
	udelay(10);
	write8(i2c1_base + I2C_ICCR, 0x81);
	write8(i2c1_base + I2C_ICSTART, 0x10);
	udelay(10);
	write8(i2c1_base + I2C_ICCR, 0x01);
	write8(i2c1_base + I2C_ICSTART, 0x00);
	udelay(10);

	/* IIC reset & enable */
	write8(base + I2C_ICCR, 0x10);
	udelay(10);
	write8(base + I2C_ICCR, 0x00);
	udelay(10);
	write8(base + I2C_ICCR, 0x10);
	udelay(10);
}
