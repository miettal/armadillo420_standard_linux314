#
# Copyright (c) 2009 Atmark Techno, Inc.  All Rights Reserved.
#

#
# Virtual
#
obj-y += memdev.o

#
# Core
#
obj-$(CONFIG_IRQ)			+= irq.o

#
# Flash
#
obj-$(CONFIG_FLASH)			+= flash_core.o
obj-$(CONFIG_FLASH_AMD)			+= flash_amd.o
obj-$(CONFIG_FLASH_INTEL)		+= flash_intel.o
obj-$(CONFIG_FLASH_SPI)			+= flash_spi.o

ifeq ($(GCC_VER), 2)
dummy-y += $(subst .o,_dummy.o,$(filter-out $(obj-y), flash_amd.o))
dummy-y += $(subst .o,_dummy.o,$(filter-out $(obj-y), flash_intel.o))
dummy-y += $(subst .o,_dummy.o,$(filter-out $(obj-y), flash_spi.o))

obj-y += $(dummy-y)
endif

#
# network
#
obj-$(CONFIG_ETHERNET)			+= net_device.o arp.o eth.o ip.o
obj-$(CONFIG_ETHERNET_EP93XX)		+= eth_ep93xx.o
obj-$(CONFIG_ETHERNET_NS9750)		+= eth_ns9750.o
obj-$(CONFIG_ETHERNET_MX25_FEC)		+= eth_mx25_fec.o
obj-$(CONFIG_ETHERNET_SMSC911X)		+= eth_smsc911x.o
obj-$(CONFIG_ETHERNET_RMA1)		+= eth_rma1.o
obj-$(CONFIG_NET_UDP)			+= udp.o
obj-$(CONFIG_NET_UDP_TFTP)		+= tftp.o

#
# i2c
#
obj-$(CONFIG_I2C_EP93XX_GPIO)		+= i2c_ep93xx_gpio.o
obj-$(CONFIG_I2C_MX25)			+= i2c_mx25.o
obj-$(CONFIG_I2C_NS9750)		+= i2c_ns9750.o
obj-$(CONFIG_I2C_RMA1)			+= i2c_rma1.o
obj-$(CONFIG_I2C_MX25_GPIO)		+= i2c_mx25_gpio.o

#
# filesystem
#
obj-$(CONFIG_FS_EXT2)			+= fs_ext2.o
obj-$(CONFIG_FS_SQUASH)			+= fs_squash.o

#
# MMC/SD
#
obj-$(CONFIG_MMCSD)			+= mmcsd.o mmcsd_block.o
obj-$(CONFIG_MMCSD_MX25_ESDHC)		+= mx25_esdhc.o
obj-$(CONFIG_MMCSD_MX31_SDHC)		+= mx31_sdhc.o
obj-$(CONFIG_MMCSD_RMA1_CORE)		+= rma1_mmcsd_core.o
obj-$(CONFIG_MMCSD_RMA1_MMC)		+= rma1_mmc.o
obj-$(CONFIG_MMCSD_RMA1_SDHC)		+= rma1_sdhc.o

#
# compact flash
#
obj-$(CONFIG_IDE_CORE)			+= ide_core.o
obj-$(CONFIG_PCMCIA_CORE)		+= pcmcia_core.o
obj-$(CONFIG_GENDISK)			+= gendisk.o

#
# EP93xx
#
obj-$(CONFIG_CPU_EP93XX)		+= ep93xx_uart.o

#
# NS9750
#
obj-$(CONFIG_CPU_NS9750)		+= ns9750_uart.o

#
# i.MX25
#
obj-$(CONFIG_CPU_IMX25)			+= mx25_clock.o
obj-$(CONFIG_CPU_IMX25)			+= mx25_iomux.o
obj-$(CONFIG_CPU_IMX25)			+= mx25_uart.o
obj-$(CONFIG_WDT_MX25)			+= mx25_wdt.o
obj-$(CONFIG_IRQ_MX25)			+= mx25_irq.o

obj-$(CONFIG_PLATFORM_ARMADILLO4X0)	+= pmic_mc34704.o
obj-$(CONFIG_PLATFORM_ARMADILLO_IOTG_STD)	+= pmic_mc34704.o

#
# i.MX31
#
obj-$(CONFIG_CPU_IMX31)			+= mx31_uart.o

#
# R-Mobile A1
#
obj-$(CONFIG_CPU_RMA1)			+= rma1_uart.o
obj-$(CONFIG_CPU_RMA1)			+= rma1_gpio.o
obj-$(CONFIG_IRQ_RMA1)			+= rma1_irq.o
obj-$(CONFIG_WDT_RMA1)			+= rma1_wdt.o

#
# miscellaneous
#
obj-$(CONFIG_EEPROM_AT88SC)		+= at88sc.o
obj-$(CONFIG_GPIO_PCA9538)		+= gpio-pca9538.o
