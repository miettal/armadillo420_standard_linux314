#define CORE_NAME "at88sc"
//#define DEBUG
#include <hermit.h>
#include <herrno.h>
#include <io.h>
#include <delay.h>
#include <arch/pinmux.h>
#include <i2c.h>
#include <rma1_gpio.h>
#include <platform/pinmux.h>

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

static inline u8 at88sc_getscl(void)
{
	gpio_direction_input(xEEPROM_SCL);
	return gpio_get_value(xEEPROM_SCL);
}

static inline void at88sc_setscl(u8 level)
{
	if (level)
		gpio_direction_input(xEEPROM_SCL);
	else
		gpio_direction_output(xEEPROM_SCL, 0);
}

static inline u8 at88sc_getsda(void)
{
	gpio_direction_input(xEEPROM_SDA);
	return gpio_get_value(xEEPROM_SDA);
}

static inline void at88sc_setsda(u8 level)
{
	if (level)
		gpio_direction_input(xEEPROM_SDA);
	else
		gpio_direction_output(xEEPROM_SDA, 0);
}

static inline u8 at88sc_sda(void)
{
	return at88sc_getsda();
}

static inline void at88sc_sdalo(void)
{
	at88sc_setsda(0);
	udelay((AT88SC_UDELAY + 1) / 2);
}

static inline void at88sc_sdahi(void)
{
	at88sc_setsda(1);
	udelay((AT88SC_UDELAY + 1) / 2);
}

static inline void at88sc_scllo(void)
{
	at88sc_setscl(0);
	udelay(AT88SC_UDELAY / 2);
}

static inline void at88sc_sclhi(void)
{
	at88sc_setscl(1);
	udelay(AT88SC_UDELAY);
}

static void at88sc_acknak(int is_ack)
{
	if (is_ack)
		at88sc_setsda(0);
	udelay((AT88SC_UDELAY + 1) / 2);
	at88sc_sclhi();
	at88sc_scllo();
}

static void at88sc_i2c_start(void)
{
	at88sc_setsda(0);
	udelay(AT88SC_UDELAY);
	at88sc_scllo();
}

static void at88sc_i2c_repstart(void)
{
	at88sc_sdahi();
	at88sc_sclhi();
	at88sc_setsda(0);
	udelay(AT88SC_UDELAY);
	at88sc_scllo();
}

static void at88sc_i2c_stop(void)
{
	at88sc_sdalo();
	at88sc_sclhi();
	at88sc_setsda(1);
	udelay(AT88SC_UDELAY);
}

static inline void at88sc_i2c_init(void)
{
	at88sc_i2c_stop();
}

#define AT88SC_ACK_TIMEOUT (20000) /* 20ms */
static int at88sc_ack_poll(void)
{
	int count = AT88SC_ACK_TIMEOUT / AT88SC_UDELAY;
	int ret = 1;

	at88sc_sdahi();
	at88sc_sclhi();

	while (at88sc_sda()) {
		if (!count) {
			ret = -H_ETIMEDOUT;
			goto poll_end;
		}
		udelay(AT88SC_UDELAY);
		count--;
	}

poll_end:
	at88sc_scllo();
	return ret;
}

static int at88sc_i2c_outb(u8 c)
{
	u8 bit;
	int i;

	for (i = 7; i >= 0; i--) {
		bit = ((c >> i) & 1);
		at88sc_setsda(bit);
		udelay((AT88SC_UDELAY + 1) / 2);

		at88sc_sclhi();
		at88sc_scllo();
	}

	return at88sc_ack_poll();
}

static u8 at88sc_i2c_inb(void)
{
	u8 indata = 0;
	int i;

	at88sc_sdahi();
	for (i = 0; i < 8; i++) {
		at88sc_sclhi();

		indata <<= 1;
		if (at88sc_sda())
			indata |= 0x01;

		at88sc_setscl(0);
		udelay((i == 7) ? (AT88SC_UDELAY / 2) : AT88SC_UDELAY);
	}

	return indata;
}

static void at88sc_i2c_readbytes(struct i2c_msg *msg)
{
	u8 *temp = msg->buf;
	int count = msg->len ? msg->len : 256;

	while (count > 0) {
		*temp = at88sc_i2c_inb();

		count--;
		temp++;

		at88sc_acknak(count);
	}
}

static int at88sc_i2c_sendbytes(struct i2c_msg *msg)
{
	const u8 *temp = msg->buf;
	int count = msg->len ? msg->len : 256;
	int repstart = 0;
	int ret;

	while (count > 0) {
		ret = at88sc_i2c_outb(*temp);

		if (ret > 0) {
			count--;
			temp++;
		} else {
			if ((count == msg->len) && !repstart) {
				at88sc_i2c_repstart();
				repstart = 1;
				continue;
			}
			return -H_EIO;
		}
	}

	return 0;
}

static int at88sc_i2c_xfer(struct i2c_msg *msgs, size_t num)
{
	int ret = 0;
	int i;

	at88sc_i2c_start();
	for (i = 0; i < num; i++) {
		if (msgs[i].flags & I2C_M_RD) {
			at88sc_i2c_readbytes(&msgs[i]);
		} else {
			ret = at88sc_i2c_sendbytes(&msgs[i]);
			if (ret)
				goto xfer_end;
		}
	}

xfer_end:
	at88sc_i2c_stop();
	return ret;
}

static int at88sc_set_user_zone(u8 zone)
{
	u8 msg[] = AT88SC_SET_USER_ZONE(zone);
	struct i2c_msg msgs[] = {
		[0] = {msg, ARRAY_SIZE(msg), I2C_M_WR},
	};

	dev_dbg("select user zone %d.\n", zone);

	return at88sc_i2c_xfer(msgs, ARRAY_SIZE(msgs));
}

static int at88sc_read_user_zone(u8 addr, u8 nr_data, u8 *data)
{
	u8 msg[] = AT88SC_READ_USER_ZONE(addr, nr_data);
	struct i2c_msg msgs[] = {
		[0] = {msg, ARRAY_SIZE(msg), I2C_M_WR},
		[1] = {data, nr_data, I2C_M_RD},
	};

	return at88sc_i2c_xfer(msgs, ARRAY_SIZE(msgs));
}

static int size_at_align(int start, int end, int align)
{
	int a;
	int b;

	if (start > end)
		return -1;

	a = align - (start % align);
	b = end - start + 1;

	return ((a < b) ? a : b);
}

int at88sc_read(int offset, u8 *buf, size_t count)
{
	int rest = count;
	int start = offset;
	u8 *data = buf;
	int nr_data;
	int addr;
	int zone;
	int ret;

	if ((offset + count) > AT88SC_USER_ZONE_SIZE) {
		dev_err("out of user zone: %d - %d\n", offset, offset + count);
		return -H_EINVAL;
	}

	while (rest) {
		zone = start / AT88SC_USER_ZONE_PER_BYTES;
		addr = start % AT88SC_USER_ZONE_PER_BYTES;
		nr_data = size_at_align(start, (start + rest - 1),
					AT88SC_USER_ZONE_PER_BYTES);

		rest -= nr_data;
		start += nr_data;

		ret = at88sc_set_user_zone(zone);
		if (ret) {
			dev_err("set user zone %d fail\n", zone);
			return ret;
		}

		ret = at88sc_read_user_zone(addr, nr_data, data);
		if (ret) {
			dev_err("read user zone %d fail: %d - %d\n",
				zone, addr, addr + (nr_data - 1));
			return ret;
		}
		data += nr_data;
	}

	return 0;
}

void at88sc_init(void)
{
	int  i;

	at88sc_sdahi();
	at88sc_sclhi();

	/* Five clock pulses must be sent before the first command is issued. */
	for (i = 0; i <= 5; i++) {
		at88sc_scllo();
		at88sc_sclhi();
	}
	mdelay(20);
}
