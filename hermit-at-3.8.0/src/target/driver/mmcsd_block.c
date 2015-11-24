#define CORE_NAME "mmcsd_block"

#include <hermit.h>
#include <io.h>
#include <string.h>
#include <mmcsd.h>
#include <gendisk.h>

static int mmc_blk_set_blksize(struct mmc_card *card)
{
	struct mmc_host *host = card->host;
	struct mmc_command cmd;

	trace();

	if (mmc_card_blockaddr(card))
		return 0;

	memset(&cmd, 0, sizeof(struct mmc_command));
	cmd.opcode = MMC_SET_BLOCKLEN;
	cmd.arg = 512;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

	return host->ops->do_cmd(host, &cmd);
}

int mmc_blk_sector_read(void *dev, u32 sector, u32 sectors, void *buf)
{
	struct mmc_card *card = (struct mmc_card *)dev;
	struct mmc_host *host = card->host;
	struct mmc_command cmd, stop;
	struct mmc_data data;
	int ret;
	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	memset(&stop, 0, sizeof(struct mmc_command));
	memset(&data, 0, sizeof(struct mmc_data));
	if (sectors == 1) {
		cmd.opcode = MMC_READ_SINGLE_BLOCK;
		data.stop = NULL;
	} else {
		cmd.opcode = MMC_READ_MULTIPLE_BLOCK;
		data.stop = &stop;
	}
	cmd.arg = sector;
	if (!mmc_card_blockaddr(card))
		cmd.arg <<= 9;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
	//cmd.debug = 1;

	cmd.data = &data;
	data.blksz = 512;
	data.blocks = sectors;
	data.flags = MMC_DATA_READ;
	data.buf = buf;

	stop.opcode = MMC_STOP_TRANSMISSION;
	stop.arg = 0;
	stop.flags = MMC_RSP_R1B | MMC_CMD_AC;
	//stop.debug = 1;

	ret = host->ops->do_cmd(host, &cmd);

	return ret;
}

int mmc_blk_sector_write(void *dev, u32 sector, u32 sectors, void *buf)
{
	struct mmc_card *card = (struct mmc_card *)dev;
	struct mmc_host *host = card->host;
	struct mmc_command cmd, stop;
	struct mmc_data data;
	int ret;
	trace();

	memset(&cmd, 0, sizeof(struct mmc_command));
	memset(&stop, 0, sizeof(struct mmc_command));
	memset(&data, 0, sizeof(struct mmc_data));
	if (sectors == 1) {
		cmd.opcode = MMC_WRITE_BLOCK;
		data.stop = NULL;
	} else {
		cmd.opcode = MMC_WRITE_MULTIPLE_BLOCK;
		data.stop = &stop;
	}
	cmd.arg = sector;
	if (!mmc_card_blockaddr(card))
		cmd.arg <<= 9;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
	//cmd.debug = 1;

	cmd.data = &data;
	data.blksz = 512;
	data.blocks = sectors;
	data.flags = MMC_DATA_WRITE;
	data.buf = buf;

	stop.opcode = MMC_STOP_TRANSMISSION;
	stop.arg = 0;
	stop.flags = MMC_RSP_R1B | MMC_CMD_AC;
	//stop.debug = 1;

	ret = host->ops->do_cmd(host, &cmd);

	return ret;
}

static char *mmcblk_diskname(struct gendisk *disk, int major, int minor)
{
	static char diskname[16];
	char *ptr = diskname;

	ptr += hsprintf(ptr, "%s", disk->name);
	if (major != -1)
		ptr += hsprintf(ptr, "%d", major);
	if (minor != -1)
		ptr += hsprintf(ptr, "p%d", minor);

	return diskname;
};

struct gendisk_ops mmcblk_ops = {
	.sector_read = mmc_blk_sector_read,
	.sector_write = mmc_blk_sector_write,
};

struct gendisk mmcblk = {
	.name = "mmcblk",
	.ops = &mmcblk_ops,
	.diskname = mmcblk_diskname,
};

struct gendisk *mmc_blk_probe(struct mmc_card *card)
{
	int ret;

	trace();

	ret = mmc_blk_set_blksize(card);
	if (ret)
		return NULL;

	mmcblk.dev = card;

	return &mmcblk;
}
