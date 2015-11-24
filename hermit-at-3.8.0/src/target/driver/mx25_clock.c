#include <arch/system.h>
#include <htypes.h>
#include <io.h>

void mx25_per_clock_enable(int gate)
{
	u32 val;
	val = read32(CCM_BASE_ADDR + CCM_CGR0);
	val |= (1 << gate);
	write32(CCM_BASE_ADDR + CCM_CGR0, val);
}

void mx25_ipg_clock_enable(int gate)
{
	u32 val;

	if (gate >= 32) {
		val = read32(CCM_BASE_ADDR + CCM_CGR2);
		val |= (1 << (gate - 32));
		write32(CCM_BASE_ADDR + CCM_CGR2, val);
	} else {
		val = read32(CCM_BASE_ADDR + CCM_CGR1);
		val |= (1 << gate);
		write32(CCM_BASE_ADDR + CCM_CGR1, val);
	}
}
