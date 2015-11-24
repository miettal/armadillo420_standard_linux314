/*
 * Copyright (C) 2013 Oskar Andero <oskar.andero@gmail.com>
 * Copyright (C) 2014 Rose Technology
 * 	   Allan Bendorff Jensen <abj@rosetechnology.dk>
 *	   Soren Andersen <san@rosetechnology.dk>
 *
 * Driver for following ADC chips from Microchip Technology's:
 * 10 Bit converter
 * MCP3001
 * MCP3002
 * MCP3004
 * MCP3008
 * ------------
 * 12 bit converter
 * MCP3201
 * MCP3202
 * MCP3204
 * MCP3208
 * ------------
 *
 * Datasheet can be found here:
 * http://ww1.microchip.com/downloads/en/DeviceDoc/21293C.pdf  mcp3001
 * http://ww1.microchip.com/downloads/en/DeviceDoc/21294E.pdf  mcp3002
 * http://ww1.microchip.com/downloads/en/DeviceDoc/21295d.pdf  mcp3004/08
 * http://ww1.microchip.com/downloads/en/DeviceDoc/21290D.pdf  mcp3201
 * http://ww1.microchip.com/downloads/en/DeviceDoc/21034D.pdf  mcp3202
 * http://ww1.microchip.com/downloads/en/DeviceDoc/21298c.pdf  mcp3204/08
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/err.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/module.h>
#include <linux/iio/buffer.h>
#include <linux/iio/iio.h>
#include <linux/iio/buffer.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>
#include <linux/regulator/consumer.h>

enum {
	mcp3001,
	mcp3002,
	mcp3004,
	mcp3008,
	mcp3201,
	mcp3202,
	mcp3204,
	mcp3208,
};

struct mcp320x_chip_info {
	const struct iio_chan_spec *channels;
	unsigned int num_channels;
	unsigned int resolution;
};

struct mcp320x {
	struct spi_device *spi;
	struct spi_message msg;
	struct spi_transfer transfer[2];

	struct regulator *reg;
	struct mutex lock;
	const struct mcp320x_chip_info *chip_info;

	u8 tx_buf ____cacheline_aligned;
	u8 rx_buf[2];

	u16 *buffer;
};

static int mcp320x_channel_to_tx_data(int device_index,
			const unsigned int channel, bool differential)
{
	int start_bit = 1;

	switch (device_index) {
	case mcp3001:
	case mcp3201:
		return 0;
	case mcp3002:
	case mcp3202:
		return ((start_bit << 4) | (!differential << 3) |
							(channel << 2));
	case mcp3004:
	case mcp3204:
	case mcp3008:
	case mcp3208:
		return ((start_bit << 6) | (!differential << 5) |
							(channel << 2));
	default:
		return -EINVAL;
	}
}

static int mcp320x_adc_conversion(struct mcp320x *adc, u8 channel,
				  bool differential, int device_index)
{
	int ret;

	adc->rx_buf[0] = 0;
	adc->rx_buf[1] = 0;
	adc->tx_buf = mcp320x_channel_to_tx_data(device_index,
						channel, differential);

	if (device_index != mcp3001 && device_index != mcp3201) {
		ret = spi_sync(adc->spi, &adc->msg);
		if (ret < 0)
			return ret;
	} else {
		ret = spi_read(adc->spi, &adc->rx_buf, sizeof(adc->rx_buf));
		if (ret < 0)
			return ret;
	}

	switch (device_index) {
	case mcp3001:
		return (adc->rx_buf[0] << 5 | adc->rx_buf[1] >> 3);
	case mcp3002:
	case mcp3004:
	case mcp3008:
		return (adc->rx_buf[0] << 2 | adc->rx_buf[1] >> 6);
	case mcp3201:
		return (adc->rx_buf[0] << 7 | adc->rx_buf[1] >> 1);
	case mcp3202:
	case mcp3204:
	case mcp3208:
		return (adc->rx_buf[0] << 4 | adc->rx_buf[1] >> 4);
	default:
		return -EINVAL;
	}
}

static int mcp320x_read_raw(struct iio_dev *indio_dev,
			    struct iio_chan_spec const *channel, int *val,
			    int *val2, long mask)
{
	struct mcp320x *adc = iio_priv(indio_dev);
	int ret = -EINVAL;
	int device_index = 0;

	mutex_lock(&adc->lock);

	device_index = spi_get_device_id(adc->spi)->driver_data;

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		if (iio_buffer_enabled(indio_dev)) {
			ret = -EBUSY;
			goto out;
		}

		ret = mcp320x_adc_conversion(adc, channel->address,
			channel->differential, device_index);

		if (ret < 0)
			goto out;

		*val = ret;
		ret = IIO_VAL_INT;
		break;

	case IIO_CHAN_INFO_SCALE:
		ret = regulator_get_voltage(adc->reg);
		if (ret < 0)
			goto out;

		/* convert regulator output voltage to mV */
		*val = ret / 1000;
		*val2 = adc->chip_info->resolution;
		ret = IIO_VAL_FRACTIONAL_LOG2;
		break;
	}

out:
	mutex_unlock(&adc->lock);

	return ret;
}

static int mcp320x_update_scan_mode(struct iio_dev *indio_dev,
				    const unsigned long *scan_mask)
{
	struct mcp320x *adc = iio_priv(indio_dev);

	if (adc->buffer)
		devm_kfree(&indio_dev->dev, adc->buffer);
	adc->buffer = devm_kzalloc(&indio_dev->dev,
				indio_dev->scan_bytes, GFP_KERNEL);
	if (!adc->buffer)
		return -ENOMEM;

	return 0;
}

#define MCP320X_VOLTAGE_CHANNEL(idx, num, res)			\
	{							\
		.type = IIO_VOLTAGE,				\
		.indexed = 1,					\
		.channel = (num),				\
		.address = (num),				\
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),	\
		.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE), \
		.scan_index = idx,				\
		.scan_type = {					\
			.sign = 'u',				\
			.realbits = (res),			\
			.storagebits = 16,			\
		},						\
	}

#define MCP320X_VOLTAGE_CHANNEL_DIFF(idx, num, res)		\
	{							\
		.type = IIO_VOLTAGE,				\
		.indexed = 1,					\
		.channel = (num * 2),				\
		.channel2 = (num * 2 + 1),			\
		.address = (num * 2),				\
		.differential = 1,				\
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),	\
		.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE), \
		.scan_index = idx,				\
		.scan_type = {					\
			.sign = 'u',				\
			.realbits = (res),			\
			.storagebits = 16,			\
		},						\
	}

static const struct iio_chan_spec mcp3001_channels[] = {
	MCP320X_VOLTAGE_CHANNEL_DIFF(0, 0, 10),
	IIO_CHAN_SOFT_TIMESTAMP(1),
};

static const struct iio_chan_spec mcp3002_channels[] = {
	MCP320X_VOLTAGE_CHANNEL(0, 0, 10),
	MCP320X_VOLTAGE_CHANNEL(1, 1, 10),
	MCP320X_VOLTAGE_CHANNEL_DIFF(2, 0, 10),
	IIO_CHAN_SOFT_TIMESTAMP(3),
};

static const struct iio_chan_spec mcp3004_channels[] = {
	MCP320X_VOLTAGE_CHANNEL(0, 0, 10),
	MCP320X_VOLTAGE_CHANNEL(1, 1, 10),
	MCP320X_VOLTAGE_CHANNEL(2, 2, 10),
	MCP320X_VOLTAGE_CHANNEL(3, 3, 10),
	MCP320X_VOLTAGE_CHANNEL_DIFF(4, 0, 10),
	MCP320X_VOLTAGE_CHANNEL_DIFF(5, 1, 10),
	IIO_CHAN_SOFT_TIMESTAMP(6),
};

static const struct iio_chan_spec mcp3008_channels[] = {
	MCP320X_VOLTAGE_CHANNEL(0, 0, 10),
	MCP320X_VOLTAGE_CHANNEL(1, 1, 10),
	MCP320X_VOLTAGE_CHANNEL(2, 2, 10),
	MCP320X_VOLTAGE_CHANNEL(3, 3, 10),
	MCP320X_VOLTAGE_CHANNEL(4, 4, 10),
	MCP320X_VOLTAGE_CHANNEL(5, 5, 10),
	MCP320X_VOLTAGE_CHANNEL(6, 6, 10),
	MCP320X_VOLTAGE_CHANNEL(7, 7, 10),
	MCP320X_VOLTAGE_CHANNEL_DIFF(8, 0, 10),
	MCP320X_VOLTAGE_CHANNEL_DIFF(9, 1, 10),
	MCP320X_VOLTAGE_CHANNEL_DIFF(10, 2, 10),
	MCP320X_VOLTAGE_CHANNEL_DIFF(11, 3, 10),
	IIO_CHAN_SOFT_TIMESTAMP(12),
};

static const struct iio_chan_spec mcp3201_channels[] = {
	MCP320X_VOLTAGE_CHANNEL_DIFF(0, 0, 12),
	IIO_CHAN_SOFT_TIMESTAMP(1),
};

static const struct iio_chan_spec mcp3202_channels[] = {
	MCP320X_VOLTAGE_CHANNEL(0, 0, 12),
	MCP320X_VOLTAGE_CHANNEL(1, 1, 12),
	MCP320X_VOLTAGE_CHANNEL_DIFF(2, 0, 12),
	IIO_CHAN_SOFT_TIMESTAMP(3),
};

static const struct iio_chan_spec mcp3204_channels[] = {
	MCP320X_VOLTAGE_CHANNEL(0, 0, 12),
	MCP320X_VOLTAGE_CHANNEL(1, 1, 12),
	MCP320X_VOLTAGE_CHANNEL(2, 2, 12),
	MCP320X_VOLTAGE_CHANNEL(3, 3, 12),
	MCP320X_VOLTAGE_CHANNEL_DIFF(4, 0, 12),
	MCP320X_VOLTAGE_CHANNEL_DIFF(5, 1, 12),
	IIO_CHAN_SOFT_TIMESTAMP(6),
};

static const struct iio_chan_spec mcp3208_channels[] = {
	MCP320X_VOLTAGE_CHANNEL(0, 0, 12),
	MCP320X_VOLTAGE_CHANNEL(1, 1, 12),
	MCP320X_VOLTAGE_CHANNEL(2, 2, 12),
	MCP320X_VOLTAGE_CHANNEL(3, 3, 12),
	MCP320X_VOLTAGE_CHANNEL(4, 4, 12),
	MCP320X_VOLTAGE_CHANNEL(5, 5, 12),
	MCP320X_VOLTAGE_CHANNEL(6, 6, 12),
	MCP320X_VOLTAGE_CHANNEL(7, 7, 12),
	MCP320X_VOLTAGE_CHANNEL_DIFF(8, 0, 12),
	MCP320X_VOLTAGE_CHANNEL_DIFF(9, 1, 12),
	MCP320X_VOLTAGE_CHANNEL_DIFF(10, 2, 12),
	MCP320X_VOLTAGE_CHANNEL_DIFF(11, 3, 12),
	IIO_CHAN_SOFT_TIMESTAMP(12),
};

static const struct iio_info mcp320x_info = {
	.read_raw = mcp320x_read_raw,
	.update_scan_mode = mcp320x_update_scan_mode,
	.driver_module = THIS_MODULE,
};

static const struct mcp320x_chip_info mcp320x_chip_infos[] = {
	[mcp3001] = {
		.channels = mcp3001_channels,
		.num_channels = ARRAY_SIZE(mcp3001_channels),
		.resolution = 10
	},
	[mcp3002] = {
		.channels = mcp3002_channels,
		.num_channels = ARRAY_SIZE(mcp3002_channels),
		.resolution = 10
	},
	[mcp3004] = {
		.channels = mcp3004_channels,
		.num_channels = ARRAY_SIZE(mcp3004_channels),
		.resolution = 10
	},
	[mcp3008] = {
		.channels = mcp3008_channels,
		.num_channels = ARRAY_SIZE(mcp3008_channels),
		.resolution = 10
	},
	[mcp3201] = {
		.channels = mcp3201_channels,
		.num_channels = ARRAY_SIZE(mcp3201_channels),
		.resolution = 12
	},
	[mcp3202] = {
		.channels = mcp3202_channels,
		.num_channels = ARRAY_SIZE(mcp3202_channels),
		.resolution = 12
	},
	[mcp3204] = {
		.channels = mcp3204_channels,
		.num_channels = ARRAY_SIZE(mcp3204_channels),
		.resolution = 12
	},
	[mcp3208] = {
		.channels = mcp3208_channels,
		.num_channels = ARRAY_SIZE(mcp3208_channels),
		.resolution = 12
	},
};

static irqreturn_t mcp320x_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct mcp320x *adc = iio_priv(indio_dev);
	struct iio_chan_spec const *channels = indio_dev->channels;
	int device_index;
	int i;

	for_each_set_bit(i, indio_dev->buffer->scan_mask,
			 indio_dev->masklength) {
		if (!test_bit(i, indio_dev->active_scan_mask))
			continue;
		device_index = spi_get_device_id(adc->spi)->driver_data;
		adc->buffer[i] = mcp320x_adc_conversion(adc, channels[i].address,
							channels[i].differential,
							device_index);
	}

	iio_push_to_buffers_with_timestamp(indio_dev, adc->buffer, pf->timestamp);

	iio_trigger_notify_done(indio_dev->trig);

	return IRQ_HANDLED;
}

static int mcp320x_probe(struct spi_device *spi)
{
	struct iio_dev *indio_dev;
	struct mcp320x *adc;
	const struct mcp320x_chip_info *chip_info;
	int ret;

	indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*adc));
	if (!indio_dev)
		return -ENOMEM;

	adc = iio_priv(indio_dev);
	adc->spi = spi;

	indio_dev->dev.parent = &spi->dev;
	indio_dev->name = spi_get_device_id(spi)->name;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->info = &mcp320x_info;

	chip_info = &mcp320x_chip_infos[spi_get_device_id(spi)->driver_data];
	indio_dev->channels = chip_info->channels;
	indio_dev->num_channels = chip_info->num_channels;

	adc->chip_info = chip_info;

	adc->transfer[0].tx_buf = &adc->tx_buf;
	adc->transfer[0].len = sizeof(adc->tx_buf);
	adc->transfer[1].rx_buf = adc->rx_buf;
	adc->transfer[1].len = sizeof(adc->rx_buf);

	spi_message_init_with_transfers(&adc->msg, adc->transfer,
					ARRAY_SIZE(adc->transfer));

	adc->reg = devm_regulator_get(&spi->dev, "vref");
	if (IS_ERR(adc->reg))
		return PTR_ERR(adc->reg);

	ret = regulator_enable(adc->reg);
	if (ret < 0)
		return ret;

	mutex_init(&adc->lock);

	ret = iio_triggered_buffer_setup(indio_dev, &iio_pollfunc_store_time,
					&mcp320x_trigger_handler, NULL);
	if (ret)
		goto reg_disable;

	ret = iio_device_register(indio_dev);
	if (ret < 0)
		goto reg_disable;

	return 0;

reg_disable:
	regulator_disable(adc->reg);

	return ret;
}

static int mcp320x_remove(struct spi_device *spi)
{
	struct iio_dev *indio_dev = spi_get_drvdata(spi);
	struct mcp320x *adc = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
	iio_triggered_buffer_cleanup(indio_dev);
	regulator_disable(adc->reg);

	return 0;
}

#if defined(CONFIG_OF)
static const struct of_device_id mcp320x_dt_ids[] = {
	{
		.compatible = "mcp3001",
		.data = &mcp320x_chip_infos[mcp3001],
	}, {
		.compatible = "mcp3002",
		.data = &mcp320x_chip_infos[mcp3002],
	}, {
		.compatible = "mcp3004",
		.data = &mcp320x_chip_infos[mcp3004],
	}, {
		.compatible = "mcp3008",
		.data = &mcp320x_chip_infos[mcp3008],
	}, {
		.compatible = "mcp3201",
		.data = &mcp320x_chip_infos[mcp3201],
	}, {
		.compatible = "mcp3202",
		.data = &mcp320x_chip_infos[mcp3202],
	}, {
		.compatible = "mcp3204",
		.data = &mcp320x_chip_infos[mcp3204],
	}, {
		.compatible = "mcp3208",
		.data = &mcp320x_chip_infos[mcp3208],
	}, {
	}
};
MODULE_DEVICE_TABLE(of, mcp320x_dt_ids);
#endif

static const struct spi_device_id mcp320x_id[] = {
	{ "mcp3001", mcp3001 },
	{ "mcp3002", mcp3002 },
	{ "mcp3004", mcp3004 },
	{ "mcp3008", mcp3008 },
	{ "mcp3201", mcp3201 },
	{ "mcp3202", mcp3202 },
	{ "mcp3204", mcp3204 },
	{ "mcp3208", mcp3208 },
	{ }
};
MODULE_DEVICE_TABLE(spi, mcp320x_id);

static struct spi_driver mcp320x_driver = {
	.driver = {
		.name = "mcp320x",
		.owner = THIS_MODULE,
	},
	.probe = mcp320x_probe,
	.remove = mcp320x_remove,
	.id_table = mcp320x_id,
};
module_spi_driver(mcp320x_driver);

MODULE_AUTHOR("Oskar Andero <oskar.andero@gmail.com>");
MODULE_DESCRIPTION("Microchip Technology MCP3x01/02/04/08");
MODULE_LICENSE("GPL v2");
