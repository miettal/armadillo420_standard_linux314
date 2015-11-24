#include <autoconf.h>
#include <hermit.h>
#include <htypes.h>
#include <arch/system.h>
#include <arch/pinmux.h>
#include <io.h>
#include "pinmux.h"
#include "proc.h"
#include "board.h"
#include "proc-at88sc.h"

void __init arch_gpio_output(unsigned int pin, unsigned char level)
{
	u8 cr;
	if (level)
		write32(PIN_GPIODSR(pin), 1 << PIN_GPIO32(pin));
	else
		write32(PIN_GPIODCR(pin), 1 << PIN_GPIO32(pin));
	cr = read8(PIN_PORTCR(pin));
	write8(PIN_PORTCR(pin), MUX(0, 1, 0, CR_PULL(cr)));
}

unsigned char __init arch_gpio_input(unsigned int pin)
{
	unsigned int val;
	u8 cr = read8(PIN_PORTCR(pin));
	write8(PIN_PORTCR(pin), MUX(0, 0, 1, CR_PULL(cr)));
	val = read32(PIN_GPIODCR(pin)) & (1 << PIN_GPIO32(pin));
	return val ? 1 : 0;
}

void __init arch_udelay(unsigned int usecs)
{
	unsigned int count;

	/* SUB = 50MHz, TCNT_CLK = 12.5MHz */
	count = (12 * usecs) + (usecs / 2) + (usecs & 1);

	write16(TCR0, 0);
	write32(TCOR0, count);
	write32(TCNT0, count);

	write8(TSTR, 1);
	while (!(read16(TCR0) & 0x100));
	write16(TCR0, 0x0000);
	write8(TSTR, 0);
}

#if defined(DEBUG)
void __init __maybe_unused arch_print_init(void)
{
	unsigned long scifa;
#if defined(CONFIG_PLATFORM_ARMADILLO800EVA)
	pinmux(PIN_FN_SCIFA1_TXD_196);
	pinmux(PIN_FN_SCIFA1_RXD_195);
	scifa = SCIFA1_BASE_ADDR;

#elif defined(CONFIG_PLATFORM_ARMADILLO810) | \
      defined(CONFIG_PLATFORM_ARMADILLO840)
	pinmux(PIN_FN_SCIFA2_TXD_201);
	pinmux(PIN_FN_SCIFA2_RXD_200);
	pinmux(PIN_FN_SCIFA2_RTS_96);
	pinmux(PIN_FN_SCIFA2_CTS_95);
	scifa = SCIFA2_BASE_ADDR;

#endif

	write16(scifa + 0x08, 0x0030);
	write16(scifa + 0x08, 0x0030);
	write16(scifa + 0x00, 0);
	write16(scifa + 0x00, 0);

	write16(scifa + 0x18, 0x0006);
	write16(scifa + 0x18, 0);
	write8(scifa + 0x04, 0x1a);
}

void __init __maybe_unused arch_print(const char *buf)
{
	char *ptr = (char *)buf;
	unsigned long scifa;
#if defined(CONFIG_PLATFORM_ARMADILLO800EVA)
	scifa = SCIFA1_BASE_ADDR;

#elif defined(CONFIG_PLATFORM_ARMADILLO810) | \
      defined(CONFIG_PLATFORM_ARMADILLO840)
	scifa = SCIFA2_BASE_ADDR;

#endif

	while (*ptr) {
		while (!((read16(scifa + 0x1c) >> 8) < 0x40));
		write8(scifa + 0x20, *(ptr++));
	}
	while ((read16(scifa + 0x1c) >> 8) & 0x7f);
}
#endif

static void __init arch_dram_init(unsigned int conf)
{
	/* DBSC */
	write32(DBCMD, 0x20000000);
	write32(DBCMD, 0x10009C40);
	arch_udelay(50);
	write32(DBKIND, 0x00000007);
	if (DRAM_ID_SIZE(conf) == DRAM_SIZE_1GB)
		write32(DBCONF0, 0x0F030A02);
	else
		write32(DBCONF0, 0x0E030A02);
	write32(DBPHYTYPE, 0x00000001);
	write32(DBBL, 0x00000000);
	write32(DBTR0, 0x00000006);
	write32(DBTR1, 0x00000005);
	write32(DBTR2, 0x00000000);
	write32(DBTR3, 0x00000006);
	write32(DBTR4, 0x00080006);
	write32(DBTR5, 0x00000015);
	write32(DBTR6, 0x0000000F);
	write32(DBTR7, 0x00000004);
	write32(DBTR8, 0x00000014);
	write32(DBTR9, 0x00000004);
	write32(DBTR10, 0x00000006);
	write32(DBTR11, 0x0000000C);
	write32(DBTR12, 0x0000000D);
	if (DRAM_ID_SIZE(conf) == DRAM_SIZE_1GB)
		write32(DBTR13, 0x00000068);
	else
		write32(DBTR13, 0x00000040);
	write32(DBTR14, 0x000A0003);
	write32(DBTR15, 0x00000003);
	write32(DBTR16, 0x40005005);
	write32(DBTR17, 0x0C0C0000);
	write32(DBTR18, 0x00000200);
	write32(DBTR19, 0x00000040);
	write32(DBRNK0, 0x00000001);
	write32(DBDFICNT, 0x00000110);
	write32(FUNCCTRL, 0x00000101);
	write32(DLLCTRL, 0x00000001);
	write32(ZQCALCTRL, 0x00000186);
	write32(ZQODTCTRL, 0xB3440051);
	write32(RDCTRL, 0x94449443);
	write32(RDTMG, 0x000000C0);
	write32(FIFOINIT, 0x00000101);
	write32(OUTCTRL, 0x02060506);
	write32(DQCALOFS1, 0x00004646);
	write32(DQCALOFS2, 0x00004646);
	write32(DQCALEXP, 0x800000AA);
	write32(DLLCTRL, 0x00000000);
	write32(DDRPNCNT, 0x00000000);

	write32(DBCMD, 0x0000000C);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DDRPNCNT, 0x00000002);

	write32(DBCMD, 0x0000000C);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(ZQCALCTRL, 0x00000187);
	write32(DBCMD, 0x00009C40);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x00009C40);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBDFICNT, 0x00000010);
	write32(OUTCTRL, 0x02060507);

	write32(DBCMD, 0x00009C40);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x21009C40);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x00009C40);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x00009C40);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x00009C40);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x00009C40);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	if (DRAM_ID_SIZE(conf) == DRAM_SIZE_1GB)
		write32(DBCMD, 0x1100006C);
	else
		write32(DBCMD, 0x11000044);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x2A000000);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x2B000000);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x29000004);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x28001520);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBCMD, 0x03000200);
	write32(DBWAIT, 0xFE400020);
	arch_udelay(50);

	write32(DBADJ2, 0x10044008);

	write32(DBRFCNF0, 0x000001FF);
	write32(DBRFCNF1, 0x00010C30);
	write32(DBRFCNF2, 0x00000000);
	write32(DBRFEN, 0x00000001);
	write32(DBACEN, 0x00000001);
}

#if defined(CONFIG_PLATFORM_ARMADILLO800EVA)
static void __init armadillo800eva_early_configure(void)
{
	/* LED1.ON, LED2/3/4.OFF */
	arch_gpio_output(xLED1, 1);
	arch_gpio_output(xLED2, 0);
	arch_gpio_output(xLED3, 0);
	arch_gpio_output(xLED4, 0);
	pinmux(xLED1);
	pinmux(xLED2);
	pinmux(xLED3);
	pinmux(xLED4);
}
#endif

static void __init arch_read_dram_id(unsigned int *dram_id)
{
	unsigned char bspif = 0;

	/* get parameter device */
	pinmux(xBSPIF0);
	pinmux(xBSPIF1);
	pinmux(xBSPIF2);
	bspif |= (arch_gpio_input(xBSPIF0) << 0);
	bspif |= (arch_gpio_input(xBSPIF1) << 1);
	bspif |= (arch_gpio_input(xBSPIF2) << 2);

	/*
	 * We know which board we are targetting at compile time.  So
	 * what we do here is to double check what we are for against
	 * what we are running on.
	 */
#if defined(CONFIG_PLATFORM_ARMADILLO800EVA)
	if (bspif != BSPIF_I2C0_EEPROM_A0)
		arch_print_initdata("WRONG BOARD");

	*dram_id = DRAM_ID(DRAM_TYPE_DDR3,
			   DRAM_SIZE_512MB,
			   DRAM_WIDTH_32BIT);

#elif defined(CONFIG_PLATFORM_ARMADILLO810) | \
      defined(CONFIG_PLATFORM_ARMADILLO840)
	if (bspif != BSPIF_CRYPTOMEMORY)
		arch_print_initdata("WRONG BOARD");

	pinmux(xEEPROM_SCL);
	pinmux(xEEPROM_SDA);
	proc_read_dram_id_from_at88sc(dram_id);
#endif

}

void __init arch_init(void)
{
	unsigned int dram_id = 0;
	unsigned char val;

	/* RWDT Setting: disable RWDT but keep a WOVF */
	val = read8(0xE6020004);
	write16(0xE6020004, 0xa500 | (val & 0x10));
#if defined(CONFIG_WDT_RMA1)
	/* RWDT enable: timeout 10s */
	write16(0xE6020000, 0x5a00);
	write16(0xE6020008, 0xa50a);
	write16(0xE6020004, 0xa587 | (val & 0x10));
#endif

	/* CPG */
	write32(RMSTPCR4, 0xff800080);
	write32(SMSTPCR4, 0xff800080);
	write32(SMSTPCR1, 0xfdffdfff);

	write32(FRQCRB, 0x00000000);
	write32(FRQCRA, 0x62030533);
	write32(FRQCRC, 0x208a354e);
	write32(FRQCRB, 0x80331050);
	while (read32(FRQCRB) & 0x80000000); /* waiting for setting complete */

	write32(FRQCRD, 0x00000000);
	write32(SUBCKCR, 0x0000010B);
	write32(PLLC01CR, 0x00004000);
	write32(PLLC2CR, 0xa0000000);
	while (!(read32(PLLC2CR) & 0x80000000));

	arch_print_init();

	arch_read_dram_id(&dram_id);

#if defined(CONFIG_PLATFORM_ARMADILLO800EVA)
	armadillo800eva_early_configure();
#endif

	/* BSC */
	write32(CMNCR, 0x0000001B);
#if defined(CONFIG_PLATFORM_ARMADILLO800EVA)
	write32(CS0BCR, 0x00410400);
#elif defined(CONFIG_PLATFORM_ARMADILLO810) | \
      defined(CONFIG_PLATFORM_ARMADILLO840)
	/* CS0: NOR flash */
	write32(CS0BCR, 0x12490400);
#endif
	write32(CS2BCR, 0x00410400);
	write32(CS5BBCR, 0x00410400);
	write32(CS6ABCR, 0x02CB0400);
	write32(CS0WCR, 0x00000440);
	write32(CS2WCR, 0x00000440);
	write32(CS5BWCR, 0x00000240);
	write32(CS6AWCR, 0x00000240);
	write32(RBWTCNT, 0x00000005);
	write32(CS0WCR2, 0x00000002);
	write32(CS2WCR2, 0x00000002);
	write32(CS4WCR2, 0x00000002);

	arch_dram_init(dram_id);
}
