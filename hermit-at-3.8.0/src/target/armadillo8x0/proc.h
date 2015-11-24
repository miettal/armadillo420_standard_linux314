/* CPG */
#define FRQCRA		0xe6150000
#define FRQCRB		0xe6150004
#define FRQCRC		0xe61500e0
#define FRQCRD		0xe61500e4
#define SUBCKCR		0xe6150080
#define PLLC01CR	0xe6150028
#define PLLC2CR		0xe615002c
#define RMSTPCR4	0xe6150120
#define SMSTPCR1	0xe6150134
#define SMSTPCR4	0xe6150140

/* BSC */
#define CMNCR		0xfec10000
#define CS0BCR		0xfec10004
#define CS2BCR		0xfec10008
#define CS4BCR		0xfec10010
#define CS5ABCR		0xfec10014
#define CS5BBCR		0xfec10018
#define CS6ABCR		0xfec1001c
#define CS0WCR		0xfec10024
#define CS2WCR		0xfec10028
#define CS4WCR		0xfec10030
#define CS5AWCR		0xfec10034
#define CS5BWCR		0xfec10038
#define CS6AWCR		0xfec1003c
#define RBWTCNT		0xfec10054
#define CS0WCR2		0xfec10224
#define CS2WCR2		0xfec10228
#define CS4WCR2		0xfec10230

/* DBSC */
#define DBACEN		0xfe400010
#define DBRFEN		0xfe400014
#define DBCMD		0xfe400018
#define DBWAIT		0xfe40001c
#define DBKIND		0xfe400020
#define DBCONF0		0xfe400024
#define DBPHYTYPE	0xfe400030
#define DBBL		0xfe4000b0
#define DBTR0		0xfe400040
#define DBTR1		0xfe400044
#define DBTR2		0xfe400048
#define DBTR3		0xfe400050
#define DBTR4		0xfe400054
#define DBTR5		0xfe400058
#define DBTR6		0xfe40005c
#define DBTR7		0xfe400060
#define DBTR8		0xfe400064
#define DBTR9		0xfe400068
#define DBTR10		0xfe40006c
#define DBTR11		0xfe400070
#define DBTR12		0xfe400074
#define DBTR13		0xfe400078
#define DBTR14		0xfe40007c
#define DBTR15		0xfe400080
#define DBTR16		0xfe400084
#define DBTR17		0xfe400088
#define DBTR18		0xfe40008c
#define DBTR19		0xfe400090
#define DBRFCNF0	0xfe4000e0
#define DBRFCNF1	0xfe4000e4
#define DBRFCNF2	0xfe4000e8
#define DBRFCNF3	0xfe4000ec
#define DBRNK0		0xfe400100
#define DBDFICNT	0xfe400244
#define DBADJ0		0xfe4000c0
#define DBADJ1		0xfe4000c4
#define DBADJ2		0xfe4000c8
#define FUNCCTRL	0xc12a0000
#define DLLCTRL		0xc12a0004
#define ZQCALCTRL	0xc12a0008
#define ZQODTCTRL	0xc12a000c
#define RDCTRL		0xc12a0010
#define RDTMG		0xc12a0014
#define FIFOINIT	0xc12a0018
#define OUTCTRL		0xc12a001c
#define DQCALOFS1	0xc12a00e8
#define DQCALOFS2	0xc12a00ec
#define DQCALEXP	0xc12a00f8
#define DDRPNCNT	0xe605803c

/* TMU */
#define TSTR		0xfff80004
#define TCOR0		0xfff80008
#define TCNT0		0xfff8000c
#define TCR0		0xfff80010

/* bspif */
#define BSPIF_I2C0_EEPROM_A0	0
#define BSPIF_CRYPTOMEMORY	1

void          __init arch_gpio_output (unsigned int  pin,
				       unsigned char level);
unsigned char __init arch_gpio_input  (unsigned int  pin);
void          __init arch_udelay      (unsigned int  usecs);


#if defined(DEBUG)
/** arch_print_initdata
 *
 * This is a trick print macro to use _before_ relocation.
 *
 * The given literal string argument is stored in the init data
 * section via static local variable. The GCC manual states in the
 * "Specifying Attributes of Variables" that "Use the section
 * attribute with global variables and not local variables", but it
 * seems to work with _static_ local variables.
 *
 * Also note that it is declared as an array of chars instead of a
 * pointer to a string.  The former specify the location of the given
 * string, instead of the location of the pointer variable holding the
 * address of the give string stored in somewhere.  It must be
 * declared as an array for this macro to work properly.
 */
#define arch_print_initdata(buf) ({ \
	static char __initdata _str[]=(buf); \
	arch_print(_str); \
})

void __init __maybe_unused arch_print_init(void);
void __init __maybe_unused arch_print(const char *buf);

#else
#define arch_print_init()
#define arch_print(buf)
#define arch_print_initdata(buf)
#endif
