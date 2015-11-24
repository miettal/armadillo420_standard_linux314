#include <hermit.h>
#include <htypes.h>
#include <herrno.h>
#include <arch/system.h>
#include <arch/pinmux.h>
#include <rma1_gpio.h>

int __maybe_unused gpio_direction_input(u32 pin)
{
	u8 val = read8(PIN_PORTCR(pin));
	if (pin == PIN_INVALID)
		return -H_EINVAL;
	write8(PIN_PORTCR(pin), MUX(0, 0, 1, CR_PULL(val)));
	return 0;
}

int __maybe_unused gpio_direction_output(u32 pin, u8 level)
{
	u8 val = read8(PIN_PORTCR(pin));
	if (pin == PIN_INVALID)
		return -H_EINVAL;
	gpio_set_value(pin, level);
	write8(PIN_PORTCR(pin), MUX(0, 1, 0, CR_PULL(val)));
	return 0;
}

int __maybe_unused gpio_get_value(u32 pin)
{
	int val;
	if (pin == PIN_INVALID)
		return -H_EINVAL;
	val = (int)read32(PIN_GPIODCR(pin)) & (1 << PIN_GPIO32(pin));
	return !!val;
}

int __maybe_unused gpio_set_value(u32 pin, u8 level)
{
	if (pin == PIN_INVALID)
		return -H_EINVAL;
	if (level)
		write32(PIN_GPIODSR(pin), 1 << PIN_GPIO32(pin));
	else
		write32(PIN_GPIODCR(pin), 1 << PIN_GPIO32(pin));
	return 0;
}
