#define CORE_NAME "pcmcia_core"

#include <autoconf.h>
#if defined(CONFIG_COMPACTFLASH_DEBUG)
#define DEBUG
#define DEBUG_DUMP_CIS
#endif
#include <hermit.h>
#include <io.h>
#include <herrno.h>
#include <htypes.h>
#include <string.h>
#include <pcmcia.h>

#define CCR_CCOR (0)
#define CCR_CCSR (2)
#define CCR_PRR  (4)
#define CCR_SCR  (6)

#define GET_CIS(o) ((u8)(pcmcia_inw(info->attr_base + ((o) << 1)) & 0xff))
#define CIS_MAX_LEN 0x200

#define SET_CCR(o, v) (pcmcia_outb(info->attr_base + ccr_base + (o), (v)))
#define GET_CCR(o) (pcmcia_inb(info->attr_base + ccr_base + (o)))

static pcmcia_info_t pcmcia_info;
static pcmcia_ops_t *_pcmcia_ops;

static inline u8
pcmcia_inb(u32 addr)
{
	return *(volatile u8 *)addr;
}

static inline u16
pcmcia_inw(u32 addr)
{
	return *(volatile u16 *)addr;
}

static inline void
pcmcia_outb(u32 addr, u8 val)
{
	*(volatile u8 *)addr = val;
}

static inline void
pcmcia_outw(u32 addr, u16 val)
{
	*(volatile u16 *)addr = val;
}

static int
pcmcia_register_ops(pcmcia_ops_t *ops)
{
	if(_pcmcia_ops)
		return -H_EINVAL;

	_pcmcia_ops = ops;
	return 0;
}

static void
pcmcia_unregister_ops(void)
{
	_pcmcia_ops = NULL;
}

static pcmcia_ops_t *
pcmcia_get_ops(void)
{
	return _pcmcia_ops;
}

#if defined(DEBUG_DUMP_CIS)
static void
pcmcia_dump_cis(pcmcia_info_t *info)
{
	u32 ptr = 0;
	int i;
	trace();
	while (ptr < CIS_MAX_LEN) {
		u8 code, len;
		if (GET_CIS(ptr) == 0xff) break;
		code = GET_CIS(ptr++);
		len = GET_CIS(ptr++);
		dev_dbg("Code: %bh, Len: %bh\n\t", code, len);
		for (i=0; i<len; i++) {
			dev_dbg_r("%b ", GET_CIS(ptr++));
			if (i % 16 == 15) dev_dbg_r("\n\t");
		}
		dev_dbg_r("\n");
	}
}
#else
#define pcmcia_dump_cis(info)
#endif

static int
pcmcia_search_tuple(pcmcia_info_t *info, u8 tuple_code)
{
	u32 ptr = 0;
	trace();
	while (ptr < CIS_MAX_LEN) {
		u8 code, len;
		if (GET_CIS(ptr) == 0xff) return -1;/* Can't find */
		if (tuple_code == GET_CIS(ptr)) return ptr;
		code = GET_CIS(ptr++);
		len = GET_CIS(ptr++);
		ptr += len;
	}
	return 0;
}

static int
pcmcia_print_card_info(pcmcia_info_t *info)
{
	u32 ptr;
	u8 code, len, ver[2];
	int i;

	ptr = pcmcia_search_tuple(info, 0x15);
	if (ptr < 0)
		return -H_EIO;

	code = GET_CIS(ptr++);
	len = GET_CIS(ptr++);
	ver[0] = GET_CIS(ptr++);
	ver[1] = GET_CIS(ptr++);

	dev_info("Disk drive detected: \n\tModel: ");
	for (i=0; i<len-2; i++) {
		u8 c = GET_CIS(ptr++);
		if (c == 0xff) break;
		if (c < 0x20) continue;
		dev_info_r("%c", c);
	}
	dev_info_r("\n");
	return 0;
}

static int
pcmcia_get_funcid(pcmcia_info_t *info)
{
	u32 ptr;
	trace();
	ptr = pcmcia_search_tuple(info, 0x21);
	if (ptr < 0)
		return -H_EIO;

	return (u32)GET_CIS(ptr + 2);
}

static int
pcmcia_config_device(pcmcia_info_t *info)
{
	pcmcia_ops_t *ops;
	u32 ptr;
	u8 size;
	u32 ccr_base = 0;
	int i;
	trace();

	ptr = pcmcia_search_tuple(info, 0x1a);
	if (ptr < 0)
		return -H_EIO;

	size = (GET_CIS(ptr + 2) & 0x3) + 1;
	for (i=0; i<size; i++)
		ccr_base |= (GET_CIS(ptr + 4 + i) << (8 * i));

	dev_dbg("ccr_base: %p\n", ccr_base);

	/* set level mode irq and I/O mapped device in config reg */
	SET_CCR(CCR_SCR,  0x00);
	SET_CCR(CCR_CCOR, 0x41);
	SET_CCR(CCR_CCSR, 0x00);
	SET_CCR(CCR_PRR,  0x00);
	dev_dbg("SCR:%b, CCOR:%b, CCSR:%b, PRR:%b\n",
		   GET_CCR(CCR_SCR), GET_CCR(CCR_CCOR),
		   GET_CCR(CCR_CCSR), GET_CCR(CCR_PRR));

	ops = pcmcia_get_ops();
	if (!ops->set_io_map)
		return -H_EINVAL;
	ops->set_io_map(1/*IO*/);

	return 0;
}

int
pcmcia_probe(struct ide_device *dev, int drive)
{
	pcmcia_ops_t *ops = (pcmcia_ops_t *)dev->ext_priv;
	int ret;
	trace();

	/* registration ops */
	if (!ops)
		return -H_EINVAL;
	if (!ops->set_socket)
		return -H_EINVAL;
	if (!ops->get_status)
		return -H_EINVAL;

	pcmcia_register_ops(ops);

	memzero(&pcmcia_info, sizeof(pcmcia_info_t));

	if (ops->hw_init) {
		ret = ops->hw_init(&pcmcia_info);
		if (ret < 0)
			return ret;
	}

	ops->set_socket(1);
	ret = ops->get_status();
	if (ret < 0)
		return -H_EIO;

	ops->set_mem_map(1/*ATTR*/);

	ret = pcmcia_get_funcid(&pcmcia_info);
	if (ret != 0x04)
		return -H_EIO;

	pcmcia_dump_cis(&pcmcia_info);
	pcmcia_print_card_info(&pcmcia_info);

	pcmcia_config_device(&pcmcia_info);

	return 0;
}

void
pcmcia_remove(struct ide_device *dev)
{
	pcmcia_ops_t *ops = (pcmcia_ops_t *)dev->ext_priv;

	trace();

	pcmcia_unregister_ops();
	if (!ops)
		return;

	if (ops->hw_free)
		ops->hw_free();
}
