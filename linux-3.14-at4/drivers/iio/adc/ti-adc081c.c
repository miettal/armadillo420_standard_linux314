/*
 * Copyright (C) 2012 Avionic Design GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>

#include <linux/iio/iio.h>
#include <linux/iio/events.h>
#include <linux/iio/adc/ti-adc081c.h>
#include <linux/regulator/consumer.h>

struct adc081c {
	struct i2c_client *i2c;
	struct regulator *ref;
	unsigned int alert_gpio;
};

#define REG_CONV_RES 0x00
#define REG_ALERT_STAT 0x01
#define REG_CONFIG 0x02
#define REG_LOW_LIM 0x03
#define REG_HIGH_LIM 0x04
#define REG_HYSTERESIS 0x05

static int adc081c_read_raw(struct iio_dev *iio,
			    struct iio_chan_spec const *channel, int *value,
			    int *shift, long mask)
{
	struct adc081c *adc = iio_priv(iio);
	int err;

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		err = i2c_smbus_read_word_swapped(adc->i2c, REG_CONV_RES);
		if (err < 0)
			return err;

		*value = (err >> 4) & 0xff;
		return IIO_VAL_INT;

	case IIO_CHAN_INFO_SCALE:
		err = regulator_get_voltage(adc->ref);
		if (err < 0)
			return err;

		*value = err / 1000;
		*shift = 8;

		return IIO_VAL_FRACTIONAL_LOG2;

	default:
		break;
	}

	return -EINVAL;
}

static int adc081c_read_event_config(struct iio_dev *iio,
				     const struct iio_chan_spec *chan,
				     enum iio_event_type type,
				     enum iio_event_direction dir)
{
	struct adc081c *adc = iio_priv(iio);
	int val;

	val = i2c_smbus_read_byte_data(adc->i2c, REG_CONFIG);
	if (val < 0)
		return val;

	return !!(val & 0x04);
}

static int adc081c_write_event_config(struct iio_dev *iio,
				      const struct iio_chan_spec *chan,
				      enum iio_event_type type,
				      enum iio_event_direction dir,
				      int state)
{
	struct adc081c *adc = iio_priv(iio);
	int val;

	val = i2c_smbus_read_byte_data(adc->i2c, REG_CONFIG);
	if (val < 0)
		return val;

	/* Alert Pin Setting */
	if (state)
		val |= 0x04;
	else
		val &= ~0x04;

	/* Conversion Interval: Tconvert * 256 */
	val &= ~0xe0;
	val |= 0x80;

	return i2c_smbus_write_byte_data(adc->i2c, REG_CONFIG, val);
}

static int adc081c_read_event_value(struct iio_dev *iio,
				    const struct iio_chan_spec *chan,
				    enum iio_event_type type,
				    enum iio_event_direction dir,
				    enum iio_event_info info,
				    int *val, int *val2)
{
	struct adc081c *adc = iio_priv(iio);
	u8 cmd;
	int err;

	switch (dir) {
	case IIO_EV_DIR_FALLING:
		cmd = REG_LOW_LIM;
		break;
	case IIO_EV_DIR_RISING:
		cmd = REG_HIGH_LIM;
		break;
	case IIO_EV_DIR_EITHER:
		cmd = REG_HYSTERESIS;
		break;
	default:
		return -EINVAL;
	}

	err = i2c_smbus_read_word_swapped(adc->i2c, cmd);
	if (err < 0)
		return err;

	*val = (err >> 4) & 0xff;

	return IIO_VAL_INT;
}

static int adc081c_write_event_value(struct iio_dev *iio,
				     const struct iio_chan_spec *chan,
				     enum iio_event_type type,
				     enum iio_event_direction dir,
				     enum iio_event_info info,
				     int val, int val2)
{
	struct adc081c *adc = iio_priv(iio);
	u8 cmd;

	switch (dir) {
	case IIO_EV_DIR_FALLING:
		cmd = REG_LOW_LIM;
		break;
	case IIO_EV_DIR_RISING:
		cmd = REG_HIGH_LIM;
		break;
	case IIO_EV_DIR_EITHER:
		cmd = REG_HYSTERESIS;
		break;
	default:
		return -EINVAL;
	}

	if (val > 0xff)
		return -EINVAL;

	val = (val & 0xff) << 4;

	return i2c_smbus_write_word_swapped(adc->i2c, cmd, val);
}

static irqreturn_t adc081c_event_handler(int irq, void *private)
{
	struct iio_dev *iio = private;
	struct adc081c *adc = iio_priv(iio);
	enum iio_event_direction ev_dir;
	u64 ev_code;
	int err;

	err = i2c_smbus_read_byte_data(adc->i2c, REG_ALERT_STAT);
	if (err < 0)
		return IRQ_HANDLED;

	switch (err & 0x3) {
	case 1:
		ev_dir = IIO_EV_DIR_FALLING;
		break;
	case 2:
		ev_dir = IIO_EV_DIR_RISING;
		break;
	case 3:
		ev_dir = IIO_EV_DIR_EITHER;
		break;
	default:
		return IRQ_NONE;
	}

	ev_code = IIO_UNMOD_EVENT_CODE(IIO_VOLTAGE, 0,
				       IIO_EV_TYPE_THRESH, ev_dir);
	iio_push_event(iio, ev_code, iio_get_time_ns());

	return IRQ_HANDLED;
}

static const struct iio_event_spec adc081c_events[] = {
	{
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_FALLING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_RISING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_EITHER,
		.mask_separate = BIT(IIO_EV_INFO_HYSTERESIS) |
			BIT(IIO_EV_INFO_ENABLE),
	},
};

static const struct iio_chan_spec adc081c_channel = {
	.type = IIO_VOLTAGE,
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
};

static const struct iio_chan_spec adc081c_channel_irq = {
	.type = IIO_VOLTAGE,
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	.event_spec = adc081c_events,
	.num_event_specs = ARRAY_SIZE(adc081c_events),
};

static const struct iio_info adc081c_info = {
	.read_raw = adc081c_read_raw,
	.driver_module = THIS_MODULE,
};

static const struct iio_info adc081c_info_irq = {
	.read_raw = adc081c_read_raw,
	.read_event_config = adc081c_read_event_config,
	.write_event_config = adc081c_write_event_config,
	.read_event_value = adc081c_read_event_value,
	.write_event_value = adc081c_write_event_value,
	.driver_module = THIS_MODULE,
};

static int adc081c_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct iio_dev *iio;
	struct adc081c *adc;
	struct adc081c_platform_data *pdata = client->dev.platform_data;
	int err;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_WORD_DATA))
		return -ENODEV;

	iio = devm_iio_device_alloc(&client->dev, sizeof(*adc));
	if (!iio)
		return -ENOMEM;

	adc = iio_priv(iio);
	adc->i2c = client;

	adc->ref = devm_regulator_get(&client->dev, "vref");
	if (IS_ERR(adc->ref))
		return PTR_ERR(adc->ref);

	err = regulator_enable(adc->ref);
	if (err < 0)
		return err;

	if (pdata && gpio_is_valid(adc->alert_gpio)) {
		adc->alert_gpio = pdata->alert_gpio;

		err = devm_gpio_request_one(&client->dev, adc->alert_gpio,
					    GPIOF_DIR_IN, "ALERT");
		if (err < 0)
			goto regulator_disable;

		err = devm_request_threaded_irq(&client->dev,
						gpio_to_irq(adc->alert_gpio),
						NULL, adc081c_event_handler,
						IRQF_TRIGGER_FALLING |
						IRQF_ONESHOT,
						"adc081c_alert", iio);
		if (err < 0)
			goto regulator_disable;
	} else
		adc->alert_gpio = -1;

	iio->dev.parent = &client->dev;
	iio->name = dev_name(&client->dev);
	iio->modes = INDIO_DIRECT_MODE;
	if (gpio_is_valid(adc->alert_gpio)) {
		iio->info = &adc081c_info_irq;
		iio->channels = &adc081c_channel_irq;
	} else {
		iio->info = &adc081c_info;
		iio->channels = &adc081c_channel;
	}
	iio->num_channels = 1;

	err = iio_device_register(iio);
	if (err < 0)
		goto regulator_disable;

	i2c_set_clientdata(client, iio);

	return 0;

regulator_disable:
	regulator_disable(adc->ref);

	return err;
}

static int adc081c_remove(struct i2c_client *client)
{
	struct iio_dev *iio = i2c_get_clientdata(client);
	struct adc081c *adc = iio_priv(iio);

	iio_device_unregister(iio);
	regulator_disable(adc->ref);

	return 0;
}

static const struct i2c_device_id adc081c_id[] = {
	{ "adc081c", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, adc081c_id);

#ifdef CONFIG_OF
static const struct of_device_id adc081c_of_match[] = {
	{ .compatible = "ti,adc081c" },
	{ }
};
MODULE_DEVICE_TABLE(of, adc081c_of_match);
#endif

static struct i2c_driver adc081c_driver = {
	.driver = {
		.name = "adc081c",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(adc081c_of_match),
	},
	.probe = adc081c_probe,
	.remove = adc081c_remove,
	.id_table = adc081c_id,
};
module_i2c_driver(adc081c_driver);

MODULE_AUTHOR("Thierry Reding <thierry.reding@avionic-design.de>");
MODULE_DESCRIPTION("Texas Instruments ADC081C021/027 driver");
MODULE_LICENSE("GPL v2");
