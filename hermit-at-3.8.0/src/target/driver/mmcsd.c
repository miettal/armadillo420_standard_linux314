#define CORE_NAME "mmcsd"

#include <autoconf.h>
#if defined(CONFIG_MMCSD_DEBUG)
#define TRACE_DEBUG
#define DEBUG
#endif
#include <hermit.h>
#include <herrno.h>
#include <platform.h>
#include <command.h>
#include <io.h>
#include <string.h>
#include <delay.h>
#include <boost.h>
#include <map.h>
#include <memdev.h>
#include <mmcsd.h>
#include <gendisk.h>
#include <gunzip.h>
#include <file.h>
#include "mmcsd_block.h"

static struct platform_info *pinfo = &platform_info;

static const unsigned int tran_exp[] = {
	10000,          100000,         1000000,        10000000,
	0,              0,              0,              0
};

static const unsigned char tran_mant[] = {
	0,      10,     12,     13,     15,     20,     25,     30,
	35,     40,     45,     50,     55,     60,     70,     80,
};

static const unsigned int tacc_exp[] = {
	1,      10,     100,    1000,   10000,  100000, 1000000, 10000000,
};
static const unsigned int tacc_mant[] = {
	0,      10,     12,     13,     15,     20,     25,     30,
	35,     40,     45,     50,     55,     60,     70,     80,
};

#define UNSTUFF_BITS(resp,start,size)                                   \
	({                                                              \
		const int __size = size;                                \
		const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1; \
		const int __off = 3 - ((start) / 32);                   \
		const int __shft = (start) & 31;                        \
		u32 __res;                                              \
									\
		__res = resp[__off] >> __shft;                          \
		if (__size + __shft > 32)                               \
			__res |= resp[__off-1] << ((32 - __shft) % 32); \
		__res & __mask;                                         \
	})

static int mmc_app_cmd(struct mmc_host *host, struct mmc_card *card)
{
	struct mmc_command cmd;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_APP_CMD;
	if (card) {
		cmd.arg = card->rca << 16;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
	} else {
		cmd.arg = 0;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_BCR;
	}
	//cmd.debug = 1;

	return host->ops->do_cmd(host, &cmd);
}

#if 0
static int mmc_sd_switch(struct mmc_host *host, struct mmc_card *card,
			 int mode, int group, u8 value)
{
	struct mmc_command cmd;
	struct mmc_data data;
	u8 buf[64];

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	memset(&data, 0, sizeof(struct mmc_data));
	memset(buf, 0, 64);
	cmd.opcode = SD_SWITCH;
	cmd.arg = mode << 31 | 0x00FFFFFF;
	cmd.arg &= ~(0xF << (group * 4));
	cmd.arg |= value << (group * 4);
	cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;

	cmd.data = &data;
	data.blksz = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;
	data.buf = buf;
	//cmd.debug = 1;

	return host->ops->do_cmd(host, &cmd);
}

static int mmc_switch_hs(struct mmc_host *host, struct mmc_card *card)
{
	return mmc_sd_switch(host, card, 1, 0, 1);
}
#endif

static int mmc_go_idle(struct mmc_host *host)
{
	struct mmc_command cmd;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_GO_IDLE_STATE;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_NONE | MMC_CMD_BC;

	return host->ops->do_cmd(host, &cmd);
}

static int mmc_send_if_cond(struct mmc_host *host, u32 ocr)
{
	static const u8 test_pattern = 0xAA;
	struct mmc_command cmd;
	int ret;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = SD_SEND_IF_COND;
	cmd.arg = ((ocr & 0xFF8000) != 0) << 8 | test_pattern;
	cmd.flags = MMC_RSP_R7 | MMC_CMD_BCR;

	ret = host->ops->do_cmd(host, &cmd);
	if (ret)
		return ret;

	if ((cmd.resp[0] & 0xff) != test_pattern)
		return -H_EIO;

	return 0;
}

static int mmc_send_op_cond(struct mmc_host *host, u32 ocr, u32 *rocr)
{
		struct mmc_command cmd;
	int ret;
	int i;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_SEND_OP_COND;
	cmd.arg = ocr;
	cmd.flags = MMC_RSP_R3 | MMC_CMD_BCR;

	for (i=10000; i; i--) {
		ret = host->ops->do_cmd(host, &cmd);
		if (ret)
			return ret;

		if (ocr == 0)
			break;

		if (cmd.resp[0] & MMC_CARD_BUSY)
			break;

		udelay(100);
	}
	if (i == 0) {
		dev_err("Card busy...\n");
		return -H_EIO;
	}

	if (rocr)
		*rocr = cmd.resp[0];

	return 0;
}

static int mmc_send_app_op_cond(struct mmc_host *host, u32 ocr, u32 *rocr)
{
	struct mmc_command cmd;
	int ret;
	int i;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = SD_APP_OP_COND;
	cmd.arg = ocr;
	cmd.flags = MMC_RSP_R3 | MMC_CMD_BCR;

	for (i=10000; i; i--) {
		ret = mmc_app_cmd(host, NULL);
		if (ret)
			break;

		ret = host->ops->do_cmd(host, &cmd);
		if (ret)
			return ret;

		if (ocr == 0)
			break;

		if (cmd.resp[0] & MMC_CARD_BUSY)
			break;

		udelay(100);
	}
	if (i == 0) {
		dev_err("Card busy...\n");
		return -H_EIO;
	}

	if (rocr)
		*rocr = cmd.resp[0];

	return ret;
}

static int mmc_all_send_cid(struct mmc_host *host, u32 *cid)
{
	struct mmc_command cmd;
	int ret;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_ALL_SEND_CID;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_R2 | MMC_CMD_BCR;
	//cmd.debug = 1;

	ret = host->ops->do_cmd(host, &cmd);
	if (ret)
		return ret;

	memcpy(cid, cmd.resp, sizeof(u32) * 4);
	return ret;
}

static int mmc_set_relative_addr(struct mmc_host *host)
{
	struct mmc_command cmd;
	int ret;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_SET_RELATIVE_ADDR;
	cmd.arg = host->card.rca << 16;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

	ret = host->ops->do_cmd(host, &cmd);
	return ret;
}

static int mmc_send_relative_addr(struct mmc_host *host, u32 *rca)
{
	struct mmc_command cmd;
	int ret;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = SD_SEND_RELATIVE_ADDR;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_R6 | MMC_CMD_BCR;

	ret = host->ops->do_cmd(host, &cmd);
	if (ret)
		return ret;

	*rca = cmd.resp[0] >> 16;

	return 0;
}

static int mmc_send_csd(struct mmc_host *host, u32 *csd)
{
	struct mmc_command cmd;
	int ret;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_SEND_CSD;
	cmd.arg = host->card.rca << 16;
	cmd.flags = MMC_RSP_R2 | MMC_CMD_AC;

	ret = host->ops->do_cmd(host, &cmd);
	if (ret)
		return ret;

	memcpy(csd, cmd.resp, sizeof(u32) * 4);

	return 0;
}

static void mmc_decode_cid(struct mmc_card *card)
{
	u32 *resp = card->raw_cid;

	memset(&card->cid, 0, sizeof(struct mmc_cid));

	/*
	 * SD doesn't currently have a version field so we will
	 * have to assume we can parse this.
	 */
	if (card->type == MMC_TYPE_MMC) {
		card->cid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
		card->cid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
		card->cid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
		card->cid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
		card->cid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
		card->cid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
	} else {
	card->cid.manfid                = UNSTUFF_BITS(resp, 120, 8);
	card->cid.oemid                 = UNSTUFF_BITS(resp, 104, 16);
	card->cid.prod_name[0]          = UNSTUFF_BITS(resp, 96, 8);
	card->cid.prod_name[1]          = UNSTUFF_BITS(resp, 88, 8);
	card->cid.prod_name[2]          = UNSTUFF_BITS(resp, 80, 8);
	card->cid.prod_name[3]          = UNSTUFF_BITS(resp, 72, 8);
	card->cid.prod_name[4]          = UNSTUFF_BITS(resp, 64, 8);
	card->cid.hwrev                 = UNSTUFF_BITS(resp, 60, 4);
	card->cid.fwrev                 = UNSTUFF_BITS(resp, 56, 4);
	card->cid.serial                = UNSTUFF_BITS(resp, 24, 32);
	card->cid.year                  = UNSTUFF_BITS(resp, 12, 8);
	card->cid.month                 = UNSTUFF_BITS(resp, 8, 4);

	card->cid.year += 2000; /* SD cards year offset */
	}
}

static int mmc_decode_csd(struct mmc_card *card)
{
	struct mmc_csd *csd = &card->csd;
	unsigned int e, m, csd_struct;
	u32 *resp = card->raw_csd;

	csd_struct = UNSTUFF_BITS(resp, 126, 2);
	if (card->type == MMC_TYPE_MMC) {
		if (csd_struct == 0)
			goto unknown_csd;
		csd_struct = 1;
	}
			
	switch (csd_struct) {
	case 0:
		m = UNSTUFF_BITS(resp, 115, 4);
		e = UNSTUFF_BITS(resp, 112, 3);
		csd->tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
		csd->tacc_clks	 = UNSTUFF_BITS(resp, 104, 8) * 100;

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr	  = tran_exp[e] * tran_mant[m];
		csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

		e = UNSTUFF_BITS(resp, 47, 3);
		m = UNSTUFF_BITS(resp, 62, 12);
		csd->capacity	  = (1 + m) << (e + 2);

		csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
		csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
		csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
		csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
		csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
		break;
	case 2:
		if (card->type != MMC_TYPE_MMC)
			goto unknown_csd;
		/* FALLTHROUGH */
	case 1:
		mmc_card_set_blockaddr(card);

		csd->tacc_ns	 = 0; /* Unused */
		csd->tacc_clks	 = 0; /* Unused */

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr	  = tran_exp[e] * tran_mant[m];
		csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

		if (card->type == MMC_TYPE_MMC) {
			e = UNSTUFF_BITS(resp, 47, 3);
			m = UNSTUFF_BITS(resp, 62, 12);
			csd->capacity	  = (1 + m) << (e + 2);

			csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
			csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
			csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
			csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
			csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
			csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
			csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
		} else {
			m = UNSTUFF_BITS(resp, 48, 22);
			csd->capacity     = (1 + m) << 10;

			csd->read_blkbits = 9;
			csd->read_partial = 0;
			csd->write_misalign = 0;
			csd->read_misalign = 0;
			csd->r2w_factor = 4; /* Unused */
			csd->write_blkbits = 9;
			csd->write_partial = 0;
		}
		break;
	default:
		goto unknown_csd;
	}

	return 0;

unknown_csd:
	dev_err("%s: unrecognised CSD structure version %d\n",
		__func__, csd_struct);
	return -H_EINVAL;
}

static int mmc_select_card(struct mmc_host *host, struct mmc_card *card)
{
	struct mmc_command cmd;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_SELECT_CARD;
	cmd.arg = card->rca << 16;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

	return host->ops->do_cmd(host, &cmd);
}

static int mmc_init_card(struct mmc_host *host, u32 ocr)
{
	struct mmc_card *card = &host->card;
	int ret;

	trace();

	memset(card, 0, sizeof(struct mmc_card));
	card->type = MMC_TYPE_MMC;
	card->rca = 1;
	card->host = host;

	mmc_go_idle(host);

	ret = mmc_send_op_cond(host, ocr | (1 << 30), NULL);
	if (ret)
		return ret;

	ret = mmc_all_send_cid(host, card->raw_cid);
	if (ret)
		return ret;

	ret = mmc_set_relative_addr(host);
	if (ret)
		return ret;

	ret = mmc_send_csd(host, card->raw_csd);
	if (ret)
		return ret;

	mmc_decode_csd(card);
	mmc_decode_cid(card);

	ret = mmc_select_card(host, card);

	//mmc_switch_hs(host, card);

	return ret;
}

static int mmc_sd_init_card(struct mmc_host *host, u32 ocr)
{
	struct mmc_card *card = &host->card;
	int ret;

	trace();

	memset(card, 0, sizeof(struct mmc_card));
	card->type = MMC_TYPE_SD;
	card->host = host;

	mmc_go_idle(host);

	ret = mmc_send_if_cond(host, ocr);
	if (!ret)
		ocr |= 1 << 30;

	ret = mmc_send_app_op_cond(host, ocr, NULL);
	if (ret)
		return ret;

	ret = mmc_all_send_cid(host, card->raw_cid);
	if (ret)
		return ret;

	ret = mmc_send_relative_addr(host, &card->rca);
	if (ret)
		return ret;

	ret = mmc_send_csd(host, card->raw_csd);
	if (ret)
		return ret;

	mmc_decode_csd(card);
	mmc_decode_cid(card);

	ret = mmc_select_card(host, card);

	//mmc_switch_hs(host, card);

	return ret;
}

static void mmc_print_card_info(struct mmc_card *card)
{
	u32 size;
	trace();

	dev_info("SD card at address %p\n", card->rca);
	size = (card->csd.capacity << (card->csd.read_blkbits - 9)) >> 1;
	dev_info("%s %dKiB\n", card->cid.prod_name, size);
}

static int mmc_switch(struct mmc_host *host, u8 set, u8 index, u8 value)
{
	struct mmc_command cmd;
	int ret;
	int i;

	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_SWITCH;
	cmd.arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
		(index << 16) |
		(value << 8) |
		set;

	cmd.flags = MMC_RSP_R1B | MMC_CMD_AC;

	for (i=3; i; i--) {
		ret = host->ops->do_cmd(host, &cmd);
		if (ret == 0)
			break;
	}

	return ret;
}

static int mmc_set_bus_width(struct mmc_host *host, struct mmc_card *card,
			     int width)
{
	struct mmc_command cmd;
	int ret;
	int i;
	u32 bus_width;

	trace();

	switch (width) {
	case 8: bus_width = EXT_CSD_BUS_WIDTH_8; break;
	case 4: bus_width = EXT_CSD_BUS_WIDTH_4; break;
	default:
	case 1: bus_width = EXT_CSD_BUS_WIDTH_1; break;
	}
	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_SWITCH;
	cmd.arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
		  (EXT_CSD_BUS_WIDTH << 16) |
		  (bus_width << 8) |
		  EXT_CSD_CMD_SET_NORMAL;
	cmd.flags = MMC_RSP_R1B | MMC_CMD_AC;
	//cmd.debug = 1;

	for (i=3; i; i--) {
		ret = host->ops->do_cmd(host, &cmd);
		if (ret == 0)
			break;
	}

	return ret;
}

static int mmc_app_set_bus_width(struct mmc_host *host, struct mmc_card *card,
				 int width)
{
	struct mmc_command cmd;
	int ret;
	int i;

	trace();
	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = SD_APP_SET_BUS_WIDTH;
	cmd.arg = width == 4 ? SD_BUS_WIDTH_4 : SD_BUS_WIDTH_1;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
	//cmd.debug = 1;

	for (i=3; i; i--) {
		ret = mmc_app_cmd(host, card);
		if (ret)
			continue;

		ret = host->ops->do_cmd(host, &cmd);
		if (ret == 0)
			break;
	}

	return ret;
}

static int mmc_scan_card(struct mmc_host *host)
{
	u32 ocr;
	int ret;

	trace();

	if (host->ops->get_cd(host) != 1) {
		dev_err("No MMC/SD card connected.\n");
		return -H_EINVAL;
	}

	host->card.type = MMC_TYPE_UNKNOWN;

	mmc_go_idle(host);
	mmc_send_if_cond(host, MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33);

	/* probing SD-Card */
	ret = mmc_send_app_op_cond(host, 0, &ocr);
	if (!ret) {
		ret = mmc_sd_init_card(host, ocr);
		if (!ret)
			return 0;
	}

	/* probing MMC-Card */
	ret = mmc_send_op_cond(host, 0, &ocr);
	if (!ret) {
		ret = mmc_init_card(host, ocr);
		if (!ret)
			return 0;
	}

	dev_err("card not found or not available data.\n");
	return -H_EINVAL;
}

static int mmc_setup_transfer_mode(struct mmc_host *host)
{
	struct mmc_card *card = &host->card;
	int width;
	int ret;

	trace();

	if (card->type == MMC_TYPE_MMC) {
		width = 8;
		ret = mmc_set_bus_width(host, card, width);
	} else {
		width = 4;
		ret = mmc_app_set_bus_width(host, card, width);
	}
	if (ret)
		return ret;

	host->ops->set_bus_width(host, width);
	dev_dbg("set_clock(max_dtr = %d)\n", card->csd.max_dtr);
	host->ops->set_clock(host, card->csd.max_dtr);

	return 0;
}

int mmcsd_load_kernel(char *device)
{
	struct mmcsd_device *dev = pinfo->mmcsd_dev;
	struct gendisk *disk;
	struct file file;
	int drive;
	int loop_start, loop_end;
	int ret;

	trace();

	if (!dev)
		return -H_ENOSYS;

	if (strncmp(device, "mmcblk", 6) != 0)
		return -H_EINVAL;
	if (device[6] < '0' || '3' < device[6])
		return -H_EINVAL;
	drive = device[6] - '0';

	switch (device[7]) {
	case '\0':
		loop_start = 0;
		loop_end = 4;
		break;
	case 'p':
		switch (device[8]) {
		case '1':
		case '2':
		case '3':
		case '4':
			loop_start = device[8] - '0' - 1;
			loop_end = device[8] - '0';
			break;
		default:
			return -H_EINVAL;
		}
		break;
	default:
		return -H_EINVAL;
	}

	ret = dev->probe(dev, drive);
	if (ret)
		return ret;

	if (dev->host->init)
		dev->host->init(dev->host);

	ret = mmc_scan_card(dev->host);
	if (ret)
		return ret;

	disk = mmc_blk_probe(&dev->host->card);
	if (!disk)
		return -H_EINVAL;;

	mmc_print_card_info(&dev->host->card);

	ret = mmc_setup_transfer_mode(dev->host);
	if (ret)
		return ret;

	ret = gendisk_find_loadable_image(disk, &file, drive,
					  loop_start, loop_end);
	if (ret)
		return ret;

	if (file.compressed)
		/* tmp buffer addr */
		file.load_addr = pinfo->map->initrd.base;
	else
		file.load_addr = pinfo->map->kernel.base;

	ret = gendisk_file_copy(disk, &file);
	if (ret)
		return ret;

	if (file.compressed)
		gunzip_object(" kernel",
			      pinfo->map->initrd.base,
			      pinfo->map->kernel.base,
			      file.size);

	if (dev->host->exit)
		dev->host->exit(dev->host);

	dev->remove(dev);

	return ret;
}

#define EMMC_DEVICE_ID 0

int mmcsd_read_data(char *device, u32 addr, u32 size, void *buf)
{
	struct mmcsd_device *dev = pinfo->mmcsd_dev;
	struct gendisk *disk;
	int blks = (size + 511) / 512;
	int ret;

	ret = dev->probe(dev, EMMC_DEVICE_ID);
	if (ret)
		return ret;

	if (dev->host->init)
		dev->host->init(dev->host);

	ret = mmc_scan_card(dev->host);
	if (ret)
		return ret;

	disk = mmc_blk_probe(&dev->host->card);
	if (!disk)
		return -H_EINVAL;;

	ret = mmc_setup_transfer_mode(dev->host);
	if (ret)
		return ret;

	mmc_switch(dev->host, EXT_CSD_CMD_SET_NORMAL, 179, 0x4a);
		
	gendisk_sector_read(disk, addr / 512, blks, buf);

	if (dev->host->exit)
		dev->host->exit(dev->host);

	dev->remove(dev);

	return 0;
}

int mmcsd_write_data(char *device, u32 addr, u32 size, void *buf)
{
	struct mmcsd_device *dev = pinfo->mmcsd_dev;
	struct gendisk *disk;
	int blks = (size + 511) / 512;
	int ret;

	ret = dev->probe(dev, EMMC_DEVICE_ID);
	if (ret)
		return ret;

	if (dev->host->init)
		dev->host->init(dev->host);

	ret = mmc_scan_card(dev->host);
	if (ret)
		return ret;

	disk = mmc_blk_probe(&dev->host->card);
	if (!disk)
		return -H_EINVAL;;

	ret = mmc_setup_transfer_mode(dev->host);
	if (ret)
		return ret;

	mmc_switch(dev->host, EXT_CSD_CMD_SET_NORMAL, 179, 0x4a);

	gendisk_sector_write(disk, 0, blks, buf);

	if (dev->host->exit)
		dev->host->exit(dev->host);

	dev->remove(dev);

	return 0;
}

static int emmcboot_memdev_read(struct memory_device *memdev,
				u32 addr, u32 size, void *buf)
{
	return mmcsd_read_data((char *)memdev->name, addr / 512, size, buf);
}

static int emmcboot_memdev_write(struct memory_device *memdev,
				 u32 addr, u32 size, void *buf)
{
	return mmcsd_write_data((char *)memdev->name, addr / 512, size, buf);
}

static int emmcboot_memdev_init(struct memory_device *dev, const char *devname)
{
	if (!dev) {
		return -H_EINVAL;
	} else {
		dev->name = devname;
		dev->type = MEMDEV_TYPE_BLOCK;
		dev->blksize = 512;
		dev->read = emmcboot_memdev_read;
		dev->write = emmcboot_memdev_write;
		dev->erase = NULL;
	}
	return 0;
}

int emmcboot0_memdev_init(struct memory_device *memdev)
{
	return emmcboot_memdev_init(memdev, "mmcblk0boot0");
}

int emmcboot1_memdev_init(struct memory_device *memdev)
{
	return emmcboot_memdev_init(memdev, "mmcblk0boot1");
}
