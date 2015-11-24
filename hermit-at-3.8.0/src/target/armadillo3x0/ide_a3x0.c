#include <htypes.h>
#include <io.h>
#include <ide.h>

#define PORT_BASE 0x62000000

static u8 _ide_inb(addr_t addr)
{
	addr_t _addr;

	_addr = PORT_BASE;
	if (addr >= 0x002c)
		_addr += 0x2c + ((addr - 0x2c) << 1);
	else
		_addr += addr << 1;
	return read8(_addr);
}

static u16 _ide_inw(addr_t addr)
{
	addr_t _addr;

	_addr = PORT_BASE;
	if (addr >= 0x002c)
		_addr += 0x2c + ((addr - 0x2c) << 1);
	else
		_addr += addr << 1;
	return read16(_addr);
}

static void _ide_outb(addr_t addr, u8 b)
{
	addr_t _addr;

	_addr = PORT_BASE;
	if (addr >= 0x002c)
		_addr += 0x2c + ((addr - 0x2c) << 1);
	else
		_addr += addr << 1;
	write8(_addr, b);
}

static void _ide_outw(addr_t addr, u16 w)
{
	addr_t _addr;

	_addr = PORT_BASE;
	if (addr >= 0x002c)
		_addr += 0x2c + ((addr - 0x2c) << 1);
	else
		_addr += addr << 1;
	write16(_addr, w);
}

#define MISC_IDESTATE            0x60000004
#define MISC_IDESTATE_CD         0x02
static int armadillo3x0_ide_probe(struct ide_device *dev, int drive)
{
	unsigned char state = read8(MISC_IDESTATE);
	return (state & MISC_IDESTATE_CD) ? 0 : -1;
}

struct ide_device armadillo3x0_ide = {
	.ext_probe = armadillo3x0_ide_probe,
	.data_port = 0x0000,
	.ctrl_port = 0x002c,
	.reset_retry = 1,

	.inb = _ide_inb,
	.inw = _ide_inw,
	.outb = _ide_outb,
	.outw = _ide_outw,
};
