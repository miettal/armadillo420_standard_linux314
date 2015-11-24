#include <herrno.h>
#include <arch/regmap.h>
#include <arch/regs_syscon.h>
#include <arch/regs_ide.h>
#include <arch/regs_smc.h>
#include <io.h>
#include <ide.h>
#include <pcmcia.h>
#include <delay.h>

static u8 _ide_inb(addr_t addr)
{
	unsigned int uiIDECR;

	/*
	 * Write the address out.
	 */
	uiIDECR = IDECtrl_DIORn | IDECtrl_DIOWn | ((addr & 7) << 2) |
		  (addr >> 10);
	write32(IDECR, uiIDECR);

	/*
	 * Toggle the read signal.
	 */
	write32(IDECR, uiIDECR & ~IDECtrl_DIORn);
	write32(IDECR, uiIDECR);

	/*
	 * Read the data in.
	 */
	return (read32(IDEDATAIN) & 0xff);
}

static u16 _ide_inw(addr_t addr)
{
	unsigned int uiIDECR;

	/*
	 * Write the address out.
	 */
	uiIDECR = IDECtrl_DIORn | IDECtrl_DIOWn | ((addr & 7) << 2) |
		  (addr >> 10);
	write32(IDECR, uiIDECR);

	/*
	 * Toggle the read signal.
	 */
	write32(IDECR, uiIDECR & ~IDECtrl_DIORn);
	write32(IDECR, uiIDECR);

	/*
	 * Read the data in.
	 */
	return (read32(IDEDATAIN) & 0xffff);
}

static void _ide_outb(addr_t addr, u8 b)
{
	unsigned int uiIDECR;

	/*
	 * Write the address out.
	 */
	uiIDECR = IDECtrl_DIORn | IDECtrl_DIOWn | ((addr & 7) << 2) |
		  (addr >> 10);
	write32(IDECR, uiIDECR);

	/*
	 * Write the data out.
	 */
	write32(IDEDATAOUT, b);

	/*
	 * Toggle the write signal.
	 */
	write32(IDECR, uiIDECR & ~IDECtrl_DIOWn);
	write32(IDECR, uiIDECR);
}

static void _ide_outw(addr_t addr, u16 w)
{
	unsigned int uiIDECR;

	/*
	 * Write the address out.
	 */
	uiIDECR = IDECtrl_DIORn | IDECtrl_DIOWn | ((addr & 7) << 2) |
		  (addr >> 10);
	write32(IDECR, uiIDECR);

	/*
	 * Write the data out.
	 */
	write32(IDEDATAOUT, w);

	/*
	 * Toggle the write signal.
	 */
	write32(IDECR, uiIDECR & ~IDECtrl_DIOWn);
	write32(IDECR, uiIDECR);
}

static void _ide_prepare(struct ide_device *dev)
{
	unsigned int uiTemp;

	/*
	 * Make sure the GPIO on IDE bits in the DEVCFG register are not set.
	 */
	uiTemp = read32(SYSCON_DEVCFG) & ~(SYSCON_DEVCFG_EonIDE |
					   SYSCON_DEVCFG_GonIDE |
					   SYSCON_DEVCFG_HonIDE);
	SysconSetLocked( SYSCON_DEVCFG, uiTemp );

	/*
	 * Make sure that MWDMA and UDMA are disabled.
	 */
	write32(IDEMDMAOP, 0);
	write32(IDEUDMAOP, 0);

	/*
	 * Set up the IDE interface for PIO transfers, using the default PIO
	 * mode.
	 */
	write32(IDECFG, IDECfg_IDEEN | IDECfg_PIO | (4 << IDECfg_MODE_SHIFT) |
		(1 << IDECfg_WST_SHIFT));
}

#define PCMCIA_BASE_PHYS	0x40000000
#define PCMCIA_ADDR_MEM		(PCMCIA_BASE_PHYS + 0xC000000)
#define PCMCIA_ADDR_ATTR	(PCMCIA_BASE_PHYS + 0x8000000)
#define PCMCIA_ADDR_IO		(PCMCIA_BASE_PHYS)

#define PCMCIA_WP		0x01
#define PCMCIA_CD1		0x02
#define PCMCIA_CD2		0x04
#define PCMCIA_BVD1		0x08
#define PCMCIA_BVD2		0x10
#define PCMCIA_VS1		0x20
#define PCMCIA_RDY		0x40
#define PCMCIA_VS2		0x80

static void _pcmcia_card_power(int on)
{
	if (on)
		write32(GPIO_PADR, read32(GPIO_PADR) & ~0x8);
	else
		write32(GPIO_PADR, read32(GPIO_PADR) | 0x8);
}

static int _pcmcia_prepare(pcmcia_info_t *info)
{
	unsigned long ulPFDR;
	unsigned long ulTemp;

	info->mem_base = PCMCIA_ADDR_MEM;
	info->attr_base = PCMCIA_ADDR_ATTR;
	info->io_base = PCMCIA_ADDR_IO;

	_pcmcia_card_power(0 /*OFF*/);
	mdelay(300);

	ulTemp = read32(GPIO_FINTEN);
	write32(GPIO_FINTEN,
		ulTemp & ~(PCMCIA_RDY | PCMCIA_CD1 |
			   PCMCIA_CD2 | PCMCIA_BVD2 | PCMCIA_BVD1));
	ulTemp = read32(GPIO_FINTEN);
	ulTemp = read32(GPIO_PFDDR);
	write32(GPIO_PFDDR,
		ulTemp & ~(PCMCIA_RDY | PCMCIA_CD1 |
			   PCMCIA_CD2 | PCMCIA_BVD2 | PCMCIA_BVD1));
	ulTemp = read32(GPIO_PFDDR);
	ulTemp = read32(GPIO_FDB);
	write32(GPIO_FDB,
		(ulTemp & ~(PCMCIA_RDY | PCMCIA_BVD2 | PCMCIA_BVD1)) |
		PCMCIA_CD1 | PCMCIA_CD2);
	ulTemp = read32(GPIO_FINTTYPE1);
	write32(GPIO_FINTTYPE1,
		(ulTemp & ~PCMCIA_RDY) |
		PCMCIA_CD1 | PCMCIA_CD2 | PCMCIA_BVD2 | PCMCIA_BVD1);
	ulTemp = read32(GPIO_FINTTYPE2);
	write32(GPIO_FINTTYPE2,
		ulTemp & ~(PCMCIA_RDY | PCMCIA_CD1 | PCMCIA_CD2 |
			   PCMCIA_BVD2 | PCMCIA_BVD1));
	ulTemp = read32(GPIO_FINTTYPE2);
	write32(GPIO_FEOI,
		PCMCIA_RDY | PCMCIA_CD1 | PCMCIA_CD2 |
		PCMCIA_BVD2 | PCMCIA_BVD1);
	ulTemp = read32(GPIO_FEOI);
	write32(SMC_PCIO, 0x800a0405);
	ulTemp = read32(SMC_PCIO);
	write32(SMC_PCAttribute, 0x110403);
	ulTemp = read32(SMC_PCAttribute);
	write32(SMC_PCCommon, 0x110403);
	ulTemp = read32(SMC_PCCommon);
	ulTemp = read32(SMC_PCMCIACtrl);
	write32(SMC_PCMCIACtrl,
		(ulTemp | PCCONT_WEN | PCCONT_PC1EN) & ~PCCONT_PC1RST);
	ulTemp = read32(SMC_PCMCIACtrl);
	ulTemp = read32(GPIO_FINTEN);
	write32(GPIO_FINTEN,
		(ulTemp & ~(PCMCIA_CD1 | PCMCIA_CD2 |
			    PCMCIA_BVD2 | PCMCIA_BVD1)) | PCMCIA_RDY);
	ulTemp = read32(GPIO_FINTEN);

	ulPFDR = read32(GPIO_PFDR);
	if (ulPFDR & (PCMCIA_CD1 | PCMCIA_CD2))
		return -H_EIO; /* No card in slot */

	_pcmcia_card_power(1 /*ON*/);
	mdelay(300);

	ulTemp = read32(SMC_PCMCIACtrl) |
		PCCONT_WEN | PCCONT_PC1EN | PCCONT_PC1RST;
	write32(SMC_PCMCIACtrl, ulTemp);
	ulTemp = (read32(SMC_PCMCIACtrl) | PCCONT_WEN | PCCONT_PC1EN) &
		~PCCONT_PC1RST;
	udelay(10);
	write32(SMC_PCMCIACtrl, ulTemp);
	ulTemp = read32(SMC_PCMCIACtrl);
	mdelay(500);

	return 0;
}

static int _pcmcia_set_socket(int power) { return 0; }
static int _pcmcia_get_status(void) { return 0; }
static int _pcmcia_set_mem_map(int map) { return 0; }
static int _pcmcia_set_io_map(int map) { return 0; }

static pcmcia_ops_t a9_pcmcia_ops = {
	.hw_init = _pcmcia_prepare,
	.set_socket = _pcmcia_set_socket,
	.get_status = _pcmcia_get_status,
	.set_mem_map = _pcmcia_set_mem_map,
	.set_io_map = _pcmcia_set_io_map,
};

static int armadillo9_ide_probe(struct ide_device *dev, int drive)
{
	dev->reset_retry = 3;
	dev->devid = drive & 1;

	switch (drive) {
	case 0: case 1:
		_ide_prepare(dev);

		dev->data_port = 0x800;
		dev->ctrl_port = 0x406;
		dev->inb = _ide_inb;
		dev->inw = _ide_inw;
		dev->outb = _ide_outb;
		dev->outw = _ide_outw;
		dev->ext_priv = NULL;
		break;
	case 2: case 3:
		dev->data_port = PCMCIA_ADDR_IO;
		dev->ctrl_port = PCMCIA_ADDR_IO + 0xe;

		dev->inb = NULL;
		dev->inw = NULL;
		dev->outb = NULL;
		dev->outw = NULL;
		dev->ext_priv = &a9_pcmcia_ops;
		return pcmcia_probe(dev, drive);
	default:
		return -H_EINVAL;
	}
	return 0;
}

struct ide_device armadillo9_ep93xx_ide = {
	.ext_probe = armadillo9_ide_probe,
};
