#define CORE_NAME "rma1_mmcsd_core"

#include <autoconf.h>
#if defined(CONFIG_MMCSD_DEBUG)
#define TRACE_DEBUG
#define DEBUG
#endif
#include <hermit.h>
#include <herrno.h>
#include <platform.h>
#include <delay.h>
#include <arch/system.h>
#include <arch/pinmux.h>
#include <mmcsd.h>

extern struct mmc_host rma1_mmc;
extern struct mmc_host rma1_sdhc;

static int rma1_mmcsd_probe(struct mmcsd_device *dev, int port)
{
	int dev_nr = 0;
	trace();

#if defined(CONFIG_MMCSD_RMA1_MMC)
	if (dev_nr++ == port) {
		dev->host = &rma1_mmc;
		dev->host->base = MMCIF_BASE_ADDR;
		return 0;
	}
#endif
#if defined(CONFIG_MMCSD_RMA1_SDHC)
	if (dev_nr++ == port) {
		dev->host = &rma1_sdhc;
		dev->host->base = SDHI0_BASE_ADDR;
		return 0;
	}
#endif
	return -H_EINVAL;
}

static void rma1_mmcsd_remove(struct mmcsd_device *dev)
{
	trace();
}

struct mmcsd_device rma1_mmcsd = {
	.probe = rma1_mmcsd_probe,
	.remove = rma1_mmcsd_remove,
};
