#include <autoconf.h>
#include <hermit.h>
#include <herrno.h>
#include <i2c.h>
#include "pinmux.h"
#include "proc.h"

#define AT88SC_NR_USER_ZONE			(4)
#define AT88SC_USER_ZONE_PER_BYTES		(0x40)
#define AT88SC_USER_ZONE_NORMAL_WRITE_WIDTH	(0x10)
#define AT88SC_USER_ZONE_SIZE	(AT88SC_NR_USER_ZONE * AT88SC_USER_ZONE_PER_BYTES)

#define AT88SC_RUZ	(0xb2)	/* Read User Zone */
#define AT88SC_SW	(0xb4)	/* System Write */

#define AT88SC_SET_USER_ZONE(_zone)	\
	{					\
		[0] = AT88SC_SW,		\
		[1] = 0x03,			\
		[2] = _zone,			\
		[3] = 0,			\
	}

#define AT88SC_READ_USER_ZONE(_addr, _nr_data)	\
	{						\
		[0] = AT88SC_RUZ,			\
		[1] = _addr,				\
		[2] = _addr,				\
		[3] = _nr_data,				\
	}

#define AT88SC_UDELAY (6)

static u8 __init __maybe_unused proc_at88sc_getscl(void)
{
	return arch_gpio_input(xEEPROM_SCL);
}

static void __init proc_at88sc_setscl(u8 level)
{
	if (level)
		arch_gpio_input(xEEPROM_SCL);
	else
		arch_gpio_output(xEEPROM_SCL, 0);
}

static u8 __init proc_at88sc_getsda(void)
{
	return arch_gpio_input(xEEPROM_SDA);
}

static void __init proc_at88sc_setsda(u8 level)
{
	if (level)
		arch_gpio_input(xEEPROM_SDA);
	else
		arch_gpio_output(xEEPROM_SDA, 0);
}

static void __init proc_at88sc_sdalo(void)
{
	proc_at88sc_setsda(0);
	arch_udelay((AT88SC_UDELAY + 1) / 2);
}

static void __init proc_at88sc_sdahi(void)
{
	proc_at88sc_setsda(1);
	arch_udelay((AT88SC_UDELAY + 1) / 2);
}

static void __init proc_at88sc_scllo(void)
{
	proc_at88sc_setscl(0);
	arch_udelay(AT88SC_UDELAY / 2);
}

static void __init proc_at88sc_sclhi(void)
{
	proc_at88sc_setscl(1);
	arch_udelay(AT88SC_UDELAY);
}

static void __init proc_at88sc_acknak(int is_ack)
{
	if (is_ack)
		proc_at88sc_setsda(0);
	arch_udelay((AT88SC_UDELAY + 1) / 2);
	proc_at88sc_sclhi();
	proc_at88sc_scllo();
}

static void __init proc_at88sc_i2c_start(void)
{
	proc_at88sc_setsda(0);
	arch_udelay(AT88SC_UDELAY);
	proc_at88sc_scllo();
}

static void __init proc_at88sc_i2c_repstart(void)
{
	proc_at88sc_sdahi();
	proc_at88sc_sclhi();
	proc_at88sc_setsda(0);
	arch_udelay(AT88SC_UDELAY);
	proc_at88sc_scllo();
}

static void __init proc_at88sc_i2c_stop(void)
{
	proc_at88sc_sdalo();
	proc_at88sc_sclhi();
	proc_at88sc_setsda(1);
	arch_udelay(AT88SC_UDELAY);
}

static void __init __maybe_unused proc_at88sc_i2c_init(void)
{
	proc_at88sc_i2c_stop();
}

#define AT88SC_ACK_TIMEOUT (20000) /* 20ms */
static int __init proc_at88sc_ack_poll(void)
{
	int count = AT88SC_ACK_TIMEOUT / AT88SC_UDELAY;
	int ret = 1;

	proc_at88sc_sdahi();
	proc_at88sc_sclhi();

	while (proc_at88sc_getsda()) {
		if (!count) {
			ret = -H_ETIMEDOUT;
			goto poll_end;
		}
		arch_udelay(AT88SC_UDELAY);
		count--;
	}

poll_end:
	proc_at88sc_scllo();
	return ret;
}

static int __init proc_at88sc_i2c_outb(u8 c)
{
	u8 bit;
	int i;

	for (i = 7; i >= 0; i--) {
		bit = ((c >> i) & 1);
		proc_at88sc_setsda(bit);
		arch_udelay((AT88SC_UDELAY + 1) / 2);

		proc_at88sc_sclhi();
		proc_at88sc_scllo();
	}

	return proc_at88sc_ack_poll();
}

static u8 __init proc_at88sc_i2c_inb(void)
{
	u8 indata = 0;
	int i;

	proc_at88sc_sdahi();
	for (i = 0; i < 8; i++) {
		proc_at88sc_sclhi();

		indata <<= 1;
		if (proc_at88sc_getsda())
			indata |= 0x01;

		proc_at88sc_setscl(0);
		arch_udelay((i == 7) ? (AT88SC_UDELAY / 2) : AT88SC_UDELAY);
	}

	return indata;
}

static void __init proc_at88sc_i2c_readbytes(struct i2c_msg *msg)
{
	u8 *temp = msg->buf;
	int count = msg->len ? msg->len : 256;

	while (count > 0) {
		*temp = proc_at88sc_i2c_inb();

		count--;
		temp++;

		proc_at88sc_acknak(count);
	}
}

static int __init proc_at88sc_i2c_sendbytes(struct i2c_msg *msg)
{
	u8 *temp = (u8 *)msg->buf;
	int count = msg->len ? msg->len : 256;
	int repstart = 0;
	int ret;

	while (count > 0) {
		ret = proc_at88sc_i2c_outb(*temp);

		if (ret > 0) {
			count--;
			temp++;
		} else {
			if ((count == msg->len) && !repstart) {
				proc_at88sc_i2c_repstart();
				repstart = 1;
				continue;
			}
			return -H_EIO;
		}
	}

	return 0;
}

static int __init proc_at88sc_i2c_xfer(struct i2c_msg *msgs, size_t num)
{
	int ret = 0;
	int i;

	proc_at88sc_i2c_start();
	for (i = 0; i < num; i++) {
		if (msgs[i].flags & I2C_M_RD) {
			proc_at88sc_i2c_readbytes(&msgs[i]);
		} else {
			ret = proc_at88sc_i2c_sendbytes(&msgs[i]);
			if (ret)
				goto xfer_end;
		}
	}

xfer_end:
	proc_at88sc_i2c_stop();
	return ret;
}

static int __init proc_at88sc_set_user_zone(u8 zone)
{
	u8 msg[] = AT88SC_SET_USER_ZONE(zone);
	struct i2c_msg msgs[] = {
		[0] = {msg, ARRAY_SIZE(msg), I2C_M_WR},
	};

	return proc_at88sc_i2c_xfer(msgs, ARRAY_SIZE(msgs));
}

static int __init proc_at88sc_read_user_zone(u8 addr, u8 nr_data, u8 *data)
{
	u8 msg[] = AT88SC_READ_USER_ZONE(addr, nr_data);
	struct i2c_msg msgs[] = {
		[0] = {msg, ARRAY_SIZE(msg), I2C_M_WR},
		[1] = {data, nr_data, I2C_M_RD},
	};

	return proc_at88sc_i2c_xfer(msgs, ARRAY_SIZE(msgs));
}

static int __init proc_size_at_align(int start, int end, int align)
{
	int a;
	int b;

	if (start > end)
		return -1;

	a = align - (start % align);
	b = end - start + 1;

	return ((a < b) ? a : b);
}

static int __init proc_at88sc_read(int offset, u8 *buf, size_t count)
{
	int rest = count;
	int start = offset;
	u8 *data = buf;
	int nr_data;
	int addr;
	int zone;
	int ret;

	if ((offset + count) > AT88SC_USER_ZONE_SIZE) {
		arch_print("at88sc: out of user zone.\r\n");
		return -H_EINVAL;
	}

	while (rest) {
		zone = start / AT88SC_USER_ZONE_PER_BYTES;
		addr = start % AT88SC_USER_ZONE_PER_BYTES;
		nr_data = proc_size_at_align(start, (start + rest - 1),
					     AT88SC_USER_ZONE_PER_BYTES);

		rest -= nr_data;
		start += nr_data;

		ret = proc_at88sc_set_user_zone(zone);
		if (ret) {
			arch_print("at88sc: failed to set user zone.\r\n");
			return ret;
		}

		ret = proc_at88sc_read_user_zone(addr, nr_data, data);
		if (ret) {
			arch_print("at88sc: failed to read data from user zone.\r\n");
			return ret;
		}
		data += nr_data;
	}

	return 0;
}

static void __init proc_at88sc_init(void)
{
	int  i;

	proc_at88sc_sdahi();
	proc_at88sc_sclhi();

	/* Five clock pulses must be sent before the first command is issued. */
	for (i = 0; i <= 5; i++) {
		proc_at88sc_scllo();
		proc_at88sc_sclhi();
	}
	arch_udelay(20 * 1000);
}

void __init proc_read_dram_id_from_at88sc(unsigned int *dram_id)
{
	proc_at88sc_init();
	proc_at88sc_read(0x30, (void *)dram_id, 4);
}
