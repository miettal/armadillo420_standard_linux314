#define CORE_NAME "ide_core"

#include <autoconf.h>
#if defined(CONFIG_COMPACTFLASH_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <platform.h>
#include <io.h>
#include <herrno.h>
#include <htypes.h>
#include <string.h>
#include <ide.h>
#include <ext2.h>
#include <delay.h>
#include <map.h>
#include <gunzip.h>
#include <gendisk.h>

/* data port */
#define IDE_DATA                 (dev->data_port + 0x00)
#define IDE_ERROR                (dev->data_port + 0x01)
#define IDE_FEAUTURES            (dev->data_port + 0x01)
#define IDE_SECTOR_COUNT         (dev->data_port + 0x02)
#define IDE_SECTOR_NUMBER        (dev->data_port + 0x03)
#define IDE_SECTOR_CYLINDER_LOW  (dev->data_port + 0x04)
#define IDE_SECTOR_CYLINDER_HIGH (dev->data_port + 0x05)
#define IDE_DEVICE_HEAD          (dev->data_port + 0x06)
#define IDE_STATUS               (dev->data_port + 0x07)
#define IDE_COMMAND              (dev->data_port + 0x07)

/* ctrl port */
#define IDE_ALTERNATE_STATUS     (dev->ctrl_port + 0x00)
#define IDE_DEVICE_CONTROL       (dev->ctrl_port + 0x00)

#define IDE_STATUS_BUSY          0x80
#define IDE_STATUS_DRDY          0x40
#define IDE_STATUS_DRQ           0x08
#define IDE_STATUS_ERR           0x01

#define IDE_COMMAND_READ_SECTORS 0x20
#define IDE_COMMAND_IDLE         0xe3
#define IDE_COMMAND_IDENTIFY     0xec

#define IDE_DEVICE_CONTROL_SRST  0x04
#define IDE_DEVICE_CONTROL_NIEN  0x02

#define IDE_LBA                  0x40

#define IDE_WAIT_TIMEOUT 500000 /* [us] */

static struct platform_info *pinfo = &platform_info;

static u8 __inb(addr_t addr)
{
	return read8(addr);
}

static u16 __inw(addr_t addr)
{
	return read16(addr);
}

static void __outb(addr_t addr, u8 val)
{
	write8(addr, val);
}

static void __outw(addr_t addr, u16 val)
{
	write16(addr, val);
}

#define ide_inb(addr) dev->inb(addr)
#define ide_inw(addr) dev->inw(addr)
#define ide_outb(addr, val) dev->outb(addr, val)
#define ide_outw(addr, val) dev->outw(addr, val)

static int
ide_wait_busy(struct ide_device *dev)
{
	int timeout = IDE_WAIT_TIMEOUT;
	u8 alt_status;

	while (timeout--) {
		alt_status = ide_inb(IDE_ALTERNATE_STATUS);
		if (!(alt_status & IDE_STATUS_BUSY))
			return 0;
		udelay(1);
	}
	hprintf("wait busy timeout\n");
	return -1;
}

static int
ide_wait_busy_and_drq(struct ide_device *dev)
{
	int timeout = IDE_WAIT_TIMEOUT;
	u8 alt_status;

	while (timeout--) {
		alt_status = ide_inb(IDE_ALTERNATE_STATUS);
		if (!(alt_status & (IDE_STATUS_BUSY | IDE_STATUS_DRQ)))
			return 0;
		udelay(1);
	}
	hprintf("wait busy and drq timeout\n");
	return -1;
}

static int
ide_wait_ready(struct ide_device *dev)
{
	int timeout = IDE_WAIT_TIMEOUT;
	u8 alt_status;

	while (timeout--) {
		alt_status = ide_inb(IDE_ALTERNATE_STATUS);
		if ((alt_status & (IDE_STATUS_DRDY)))
			return 0;
		udelay(1);
	}
	hprintf("wait ready timeout\n");
	return -1;
}

static int
ide_reset(struct ide_device *dev)
{
	u8 error;
	int ret;

	trace();

	/* disable drive interrupts during IDE probe */
	ide_outb(IDE_DEVICE_CONTROL,
		 IDE_DEVICE_CONTROL_SRST |IDE_DEVICE_CONTROL_NIEN );
	mdelay(50);

	ide_outb(IDE_DEVICE_CONTROL, IDE_DEVICE_CONTROL_NIEN);
	mdelay(50);

	ret = ide_wait_busy(dev);
	if (ret < 0)
		return -H_EIO;

	error = ide_inb(IDE_ERROR);
	if (error & 0xfe)
		return -H_EIO;

	return 0;
}

static int
ide_device_selection(struct ide_device *dev)
{
	int ret;

	ret = ide_wait_busy_and_drq(dev);
	if (ret < 0)
		return -H_EIO;

	ide_outb(IDE_DEVICE_CONTROL, IDE_DEVICE_CONTROL_NIEN);
	ide_outb(IDE_DEVICE_HEAD, dev->devid << 4);
	udelay(1);

	ret = ide_wait_busy_and_drq(dev);
	if (ret < 0)
		return -H_EIO;

	return 0;
}

static int
ide_register_check(struct ide_device *dev)
{
	u8 cyl_low, cyl_high;
	trace();
	ide_outb(IDE_SECTOR_CYLINDER_LOW, 0x55);
	ide_outb(IDE_SECTOR_CYLINDER_HIGH, 0xaa);
	cyl_low = ide_inb(IDE_SECTOR_CYLINDER_LOW);
	cyl_high = ide_inb(IDE_SECTOR_CYLINDER_HIGH);
	if (cyl_low != 0x55 || cyl_high != 0xaa)
		return -H_EIO;

	return 0;
}

static int
ide_idle(struct ide_device *dev)
{
	int ret;
	trace();
	ret = ide_device_selection(dev);
	if (ret < 0)
		return -H_EIO;

	ret = ide_wait_ready(dev);
	if (ret < 0)
		return -H_EIO;

	ide_outb(IDE_SECTOR_COUNT, 0x00);
	ide_outb(IDE_COMMAND, IDE_COMMAND_IDLE);
	udelay(1);

	ret = ide_wait_busy_and_drq(dev);
	if (ret < 0)
		return -H_EIO;

	return 0;
}

static int
ide_read_data(struct ide_device *dev, u8 *buf, int count)
{
	u16 data;
	u8 status;
	int ret;
	int i;

	while (count--) {
		ret = ide_wait_busy(dev);
		if (ret < 0)
			return -H_EIO;

		status = ide_inb(IDE_ALTERNATE_STATUS);
		if (status & IDE_STATUS_ERR) {
			u8 err = ide_inb(IDE_ERROR);
			hprintf("status: %p, error: %p\n", status, err);
			return -H_EIO;
		}

		for (i=0; i<256; i++) {
			data = ide_inw(IDE_DATA);
			*buf++ = ((data) & 0xff);
			*buf++ = ((data >> 8) & 0xff);
		}
	}

	return 0;
}

static void
ide_print_identify(struct ide_device *dev, u8 *buf)
{
	int i;

	dev_info("Disk drive detected: \n");
	dev_info_r("\tModel: ");
	for (i=27*2; i<46*2; i+=2)
		dev_info_r("%c%c", buf[i+1], buf[i]);

	dev_info_r("\n\tRev.: ");
	for (i=23*2; i<26*2; i+=2)
		dev_info_r("%c%c", buf[i+1], buf[i]);

	dev_info_r("\n\tSerial NO.: ");
	for (i=10*2; i<19*2; i+=2)
		dev_info_r("%c%c", buf[i+1], buf[i]);

	dev_info_r("\n");
}

static int
ide_identify_device(struct ide_device *dev)
{
	u16 buf[256];
	int ret;

	trace();

	ret = ide_wait_ready(dev);
	if (ret < 0)
		return -H_EIO;

	ide_outb(IDE_COMMAND, IDE_COMMAND_IDENTIFY);
	udelay(1);

	ret = ide_read_data(dev, (u8 *)buf, 1);
	if (ret < 0)
		return -H_EIO;

	if (buf[0] & 0x04)
		return -H_EIO; /* COMMAND_IDENTIFY improperly */

	ide_print_identify(dev, (u8 *)buf);

	return 0;
}

static int
ide_sector_read(void *gdev, u32 sector, u32 nr_sectors, u8 *buf)
{
	struct ide_device *dev = gdev;
	int ret;

	ret = ide_device_selection(dev);
	if (ret < 0)
		return -H_EIO;

	ret = ide_wait_ready(dev);
	if (ret < 0)
		return -H_EIO;

	ide_outb(IDE_SECTOR_COUNT, nr_sectors);
	ide_outb(IDE_SECTOR_NUMBER, sector & 0xff);
	ide_outb(IDE_SECTOR_CYLINDER_LOW, (sector >> 8) & 0xff);
	ide_outb(IDE_SECTOR_CYLINDER_HIGH, (sector >> 16) & 0xff);
	ide_outb(IDE_DEVICE_HEAD,
		 IDE_LBA | (dev->devid << 4) | ((sector >> 24) & 0x0f));
	ide_outb(IDE_COMMAND, IDE_COMMAND_READ_SECTORS);
	udelay(1);

	return ide_read_data(dev, buf, nr_sectors);
}

static char *ide_diskname(struct gendisk *disk, int drive, int pno)
{
	static char diskname[16];
	char *ptr = diskname;

	ptr += hsprintf(ptr, "hd%c%d", 'a' + drive, pno);

	return diskname;
}

static struct gendisk_ops ide_ops = {
	.sector_read = ide_sector_read,
};

static struct gendisk ide_disk = {
	.name = "ide",
	.ops = &ide_ops,
	.diskname = ide_diskname,
};

struct gendisk *
ide_probe(struct ide_device *dev, int drive)
{
	int ret;
	int retry;
	trace();

	if (dev->ext_probe) {
		ret = dev->ext_probe(dev, drive);
		if (ret < 0)
			return NULL;
	}

	if (!dev->inb)
		dev->inb = __inb;
	if (!dev->inw)
		dev->inw = __inw;
	if (!dev->outb)
		dev->outb = __outb;
	if (!dev->outw)
		dev->outw = __outw;

	for (retry = dev->reset_retry, ret = -H_EIO; retry > 0 && ret; retry--)
		ret = ide_reset(dev);
	if (ret < 0)
		return NULL;

	ret = ide_device_selection(dev);
	if (ret < 0)
		return NULL;

	ret = ide_register_check(dev);
	if (ret < 0)
		return NULL;

	ide_identify_device(dev);

	if (ide_idle(dev))
		return NULL;

	ide_disk.dev = dev;

	return &ide_disk;
}

void
ide_remove(struct ide_device *dev)
{
	trace();
	if (dev->ext_remove)
		dev->ext_remove(dev);
}

int ide_load_kernel(char *device)
{
	struct ide_device *dev = pinfo->ide_dev;
	struct gendisk *disk;
	file_t file;
	int success = 0;
	int drive = 0;
	int loop_start = 0, loop_end = 0;
	int ret = 0;
	int i;
	addr_t temp_addr = pinfo->map->initrd.base;
	addr_t kernel_addr = pinfo->map->kernel.base;

	trace();

	for (i=0; i<4; i++) {
		char hd[4] = "hda";
		hd[2] = 'a' + i;
		if (strncmp(device, hd, 3) == 0) {
			drive = i;
			if (device[3] == '\0') {
				loop_start = 0;
				loop_end = 4;
			} else {
				loop_start = device[3] - '0' - 1;
				loop_end = device[3] - '0';
			}
		}
	}

	disk = ide_probe(dev, drive);
	if (disk == NULL) {
		ret = -H_EINVAL;
		goto err_out;
	}

	ret = gendisk_find_loadable_image(disk, &file, drive,
					  loop_start, loop_end);
	if (ret < 0)
		goto err_out;

	if (file.size > (temp_addr - kernel_addr)) {
		dev_err("%s size is too large.\n", file.name);
		goto err_out;
	}

	if (file.compressed)
		/* tmp buffer addr */
		file.load_addr = temp_addr;
	else
		file.load_addr = kernel_addr;

	ret = gendisk_file_copy(disk, &file);
	if (ret == 0)
		success = 1;

	ide_remove(dev);

	if (!success) {
		dev_err("Not found bootable image.\n");
		return -H_EIO;
	}

	if (file.compressed)
		gunzip_object(" kernel",
			      temp_addr,
			      kernel_addr,
			      file.size);

	return 0;

err_out:
	ide_remove(dev);

	return ret;
}
