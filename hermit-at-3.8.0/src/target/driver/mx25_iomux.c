#include <arch/system.h>
#include <io.h>

void mx25_iomux_config(struct iomux_info *iomux, int type)
{
	u32 base = IOMUXC_BASE_ADDR;
	u8 mux;
	u16 pad;
	int i;
	for (i=0; ; i++) {
		if (!iomux[i].pin)
			break;

		/* MUX */
		if (PIN_TO_IOMUX_MUX(iomux[i].pin) != NON_MUX_I) {
			if (type == 1)
				mux = iomux[i].mux1;
			else
				mux = iomux[i].mux2;
			write32(base + PIN_TO_IOMUX_MUX(iomux[i].pin),
				(u32)mux);
		}

		/* PAD */
		if (!PIN_TO_IOMUX_PAD(iomux[i].pin))
			continue;

		if (type == 1)
			pad = iomux[i].pad1;
		else
			pad = iomux[i].pad2;
		if (pad == (u16)-1)
			continue;
		write32(base + PIN_TO_IOMUX_PAD(iomux[i].pin), (u32)pad);
	}
}

static addr_t gpio_base[4] = {
	GPIO1_BASE_ADDR,
	GPIO2_BASE_ADDR,
	GPIO3_BASE_ADDR,
	GPIO4_BASE_ADDR
};

void mx25_gpio_output(int pin, int mode)
{
	u32 val;

	val = read32(gpio_base[PIN_TO_GPIOPORT(pin)] + GPIO(GDIR));
	val |= (1 << PIN_TO_GPIOBIT(pin));
	write32(gpio_base[PIN_TO_GPIOPORT(pin)] + GPIO(GDIR), val);

	val = read32(gpio_base[PIN_TO_GPIOPORT(pin)] + GPIO(DR));
	if (mode)
		val |= (1 << PIN_TO_GPIOBIT(pin));
	else
		val &= ~(1 << PIN_TO_GPIOBIT(pin));
	write32(gpio_base[PIN_TO_GPIOPORT(pin)] + GPIO(DR), val);
}

void mx25_gpio_input(int pin)
{
	u32 val;

	val = read32(gpio_base[PIN_TO_GPIOPORT(pin)] + GPIO(GDIR));
	val &= ~(1 << PIN_TO_GPIOBIT(pin));
	write32(gpio_base[PIN_TO_GPIOPORT(pin)] + GPIO(GDIR), val);
}

int mx25_gpio_get_psr(int pin)
{
	u32 val;

	mx25_gpio_input(pin);

	val = read32(gpio_base[PIN_TO_GPIOPORT(pin)] + GPIO(PSR));
	if (val & (1 << PIN_TO_GPIOBIT(pin)))
		return 1;
	else
		return 0;
}
