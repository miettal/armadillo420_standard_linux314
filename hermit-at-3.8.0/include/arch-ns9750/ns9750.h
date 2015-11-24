/*
 *  linux/include/asm-arm/hardware/ns9750.h
 *
 *  Copyright (C) 2004 Atmark Techno, Inc.
 */
#ifndef	__ASM_HARDWARE_NS9750_H

#define	NS9750_SCM_PHYS_BASE	(0xa0900000)
#define NS9750_SCM_SIZE		(0x00100000)
#define	NS9750_MEM_PHYS_BASE	(0xa0700000)
#define NS9750_MEM_SIZE		(0x00100000)
#define	NS9750_ETH_PHYS_BASE	(0xa0600000)
#define NS9750_ETH_SIZE		(0x00100000)
#define	NS9750_BBB_PHYS_BASE	(0xa0400000)
#define NS9750_BBB_SIZE		(0x00100000)
#define	NS9750_DMAUSB_PHYS_BASE	(0x90000000)
#define NS9750_DMAUSB_SIZE	(0x00200000)
#define	NS9750_BBU_PHYS_BASE	(0x90600000)
#define NS9750_BBU_SIZE		(0x00100000)
#define	NS9750_I2C_PHYS_BASE	(0x90500000)
#define NS9750_I2C_SIZE		(0x00100000)
#define	NS9750_LCD_PHYS_BASE	(0xa0800000)
#define NS9750_LCD_SIZE		(0x00100000)
#define	NS9750_SER_PHYS_BASE	(0x90200000)
#define NS9750_SER_SIZE		(0x00200000)
#define	NS9750_BSD_PHYS_BASE	(0x90400000)
#define NS9750_BSD_SIZE		(0x00100000)

#define NS9750_PCIMEM_PHYS_BASE	(0x80000000)
#define NS9750_PCIMEM_SIZE	(0x10000000)
#define NS9750_PCI_IO_PHYS_BASE	(0xa0000000)
#define NS9750_PCI_IO_SIZE	(0x00100000)
#define NS9750_PCI_CA_PHYS_BASE	(0xa0100000)
#define NS9750_PCI_CA_SIZE	(0x00100000)
#define NS9750_PCI_CD_PHYS_BASE	(0xa0200000)
#define NS9750_PCI_CD_SIZE	(0x00100000)
#define NS9750_PCI_PHYS_BASE	(0xa0300000)
#define NS9750_PCI_SIZE		(0x00100000)

#define NS9750_IO_PHYS_BASE	(0x90000000)

#ifndef	__ASSEMBLY__
#define	get_scm_reg_addr(c)	((volatile unsigned int *)(NS9750_SCM_PHYS_BASE + (c)))
#define	get_mem_reg_addr(c)	((volatile unsigned int *)(NS9750_MEM_PHYS_BASE + (c)))
#define	get_eth_reg_addr(c)	((volatile unsigned int *)(NS9750_ETH_PHYS_BASE + (c)))
#define	get_dma_reg_addr(c)	((volatile unsigned int *)(NS9750_DMA_PHYS_BASE + (c)))
#define	get_bbb_reg_addr(c)	((volatile unsigned int *)(NS9750_BBB_PHYS_BASE + (c)))
#define	get_bbu_reg_addr(c)	((volatile unsigned int *)(NS9750_BBU_PHYS_BASE + (c)))
#define	get_i2c_reg_addr(c)	((volatile unsigned int *)(NS9750_I2C_PHYS_BASE + (c)))
#define	get_lcd_reg_addr(c)	((volatile unsigned int *)(NS9750_LCD_PHYS_BASE + (c)))
#define	get_ser_reg_addr(c)	((volatile unsigned int *)(NS9750_SER_PHYS_BASE + (c)))
#define	get_bsd_reg_addr(c)	((volatile unsigned int *)(NS9750_BSD_PHYS_BASE + (c)))
#define	get_usb_reg_addr(c)	((volatile unsigned int *)(NS9750_USB_PHYS_BASE + (c)))
#define	get_pci_reg_addr(c)	((volatile unsigned int *)(NS9750_PCI_PHYS_BASE + (c)))

#define	ns_scm_readb(off)	__raw_readb(NS9750_SCM_PHYS_BASE + (off))
#define	ns_scm_readw(off)	__raw_readw(NS9750_SCM_PHYS_BASE + (off))
#define	ns_scm_readl(off)	__raw_readl(NS9750_SCM_PHYS_BASE + (off))
#define	ns_scm_writeb(val,off)	__raw_writeb(val, NS9750_SCM_PHYS_BASE + (off))
#define	ns_scm_writew(val,off)	__raw_writew(val, NS9750_SCM_PHYS_BASE + (off))
#define	ns_scm_writel(val,off)	__raw_writel(val, NS9750_SCM_PHYS_BASE + (off))
#define	ns_mem_readb(off)	__raw_readb(NS9750_MEM_PHYS_BASE + (off))
#define	ns_mem_readw(off)	__raw_readw(NS9750_MEM_PHYS_BASE + (off))
#define	ns_mem_readl(off)	__raw_readl(NS9750_MEM_PHYS_BASE + (off))
#define	ns_mem_writeb(val,off)	__raw_writeb(val, NS9750_MEM_PHYS_BASE + (off))
#define	ns_mem_writew(val,off)	__raw_writew(val, NS9750_MEM_PHYS_BASE + (off))
#define	ns_mem_writel(val,off)	__raw_writel(val, NS9750_MEM_PHYS_BASE + (off))
#define	ns_eth_readb(off)	__raw_readb(NS9750_ETH_PHYS_BASE + (off))
#define	ns_eth_readw(off)	__raw_readw(NS9750_ETH_PHYS_BASE + (off))
#define	ns_eth_readl(off)	__raw_readl(NS9750_ETH_PHYS_BASE + (off))
#define	ns_eth_writeb(val,off)	__raw_writeb(val, NS9750_ETH_PHYS_BASE + (off))
#define	ns_eth_writew(val,off)	__raw_writew(val, NS9750_ETH_PHYS_BASE + (off))
#define	ns_eth_writel(val,off)	__raw_writel(val, NS9750_ETH_PHYS_BASE + (off))
#define	ns_dma_readb(off)	__raw_readb(NS9750_DMA_PHYS_BASE + (off))
#define	ns_dma_readw(off)	__raw_readw(NS9750_DMA_PHYS_BASE + (off))
#define	ns_dma_readl(off)	__raw_readl(NS9750_DMA_PHYS_BASE + (off))
#define	ns_dma_writeb(val,off)	__raw_writeb(val, NS9750_DMA_PHYS_BASE + (off))
#define	ns_dma_writew(val,off)	__raw_writew(val, NS9750_DMA_PHYS_BASE + (off))
#define	ns_dma_writel(val,off)	__raw_writel(val, NS9750_DMA_PHYS_BASE + (off))
#define	ns_bbb_readb(off)	__raw_readb(NS9750_BBB_PHYS_BASE + (off))
#define	ns_bbb_readw(off)	__raw_readw(NS9750_BBB_PHYS_BASE + (off))
#define	ns_bbb_readl(off)	__raw_readl(NS9750_BBB_PHYS_BASE + (off))
#define	ns_bbb_writeb(val,off)	__raw_writeb(val, NS9750_BBB_PHYS_BASE + (off))
#define	ns_bbb_writew(val,off)	__raw_writew(val, NS9750_BBB_PHYS_BASE + (off))
#define	ns_bbb_writel(val,off)	__raw_writel(val, NS9750_BBB_PHYS_BASE + (off))
#define	ns_bbu_readb(off)	__raw_readb(NS9750_BBU_PHYS_BASE + (off))
#define	ns_bbu_readw(off)	__raw_readw(NS9750_BBU_PHYS_BASE + (off))
#define	ns_bbu_readl(off)	__raw_readl(NS9750_BBU_PHYS_BASE + (off))
#define	ns_bbu_writeb(val,off)	__raw_writeb(val, NS9750_BBU_PHYS_BASE + (off))
#define	ns_bbu_writew(val,off)	__raw_writew(val, NS9750_BBU_PHYS_BASE + (off))
#define	ns_bbu_writel(val,off)	__raw_writel(val, NS9750_BBU_PHYS_BASE + (off))
#define	ns_i2c_readb(off)	__raw_readb(NS9750_I2C_PHYS_BASE + (off))
#define	ns_i2c_readw(off)	__raw_readw(NS9750_I2C_PHYS_BASE + (off))
#define	ns_i2c_readl(off)	__raw_readl(NS9750_I2C_PHYS_BASE + (off))
#define	ns_i2c_writeb(val,off)	__raw_writeb(val, NS9750_I2C_PHYS_BASE + (off))
#define	ns_i2c_writew(val,off)	__raw_writew(val, NS9750_I2C_PHYS_BASE + (off))
#define	ns_i2c_writel(val,off)	__raw_writel(val, NS9750_I2C_PHYS_BASE + (off))
#define	ns_lcd_readb(off)	__raw_readb(NS9750_LCD_PHYS_BASE + (off))
#define	ns_lcd_readw(off)	__raw_readw(NS9750_LCD_PHYS_BASE + (off))
#define	ns_lcd_readl(off)	__raw_readl(NS9750_LCD_PHYS_BASE + (off))
#define	ns_lcd_writeb(val,off)	__raw_writeb(val, NS9750_LCD_PHYS_BASE + (off))
#define	ns_lcd_writew(val,off)	__raw_writew(val, NS9750_LCD_PHYS_BASE + (off))
#define	ns_lcd_writel(val,off)	__raw_writel(val, NS9750_LCD_PHYS_BASE + (off))
#define	ns_ser_readb(off)	__raw_readb(NS9750_SER_PHYS_BASE + (off))
#define	ns_ser_readw(off)	__raw_readw(NS9750_SER_PHYS_BASE + (off))
#define	ns_ser_readl(off)	__raw_readl(NS9750_SER_PHYS_BASE + (off))
#define	ns_ser_writeb(val,off)	__raw_writeb(val, NS9750_SER_PHYS_BASE + (off))
#define	ns_ser_writew(val,off)	__raw_writew(val, NS9750_SER_PHYS_BASE + (off))
#define	ns_ser_writel(val,off)	__raw_writel(val, NS9750_SER_PHYS_BASE + (off))
#define	ns_bsd_readb(off)	__raw_readb(NS9750_BSD_PHYS_BASE + (off))
#define	ns_bsd_readw(off)	__raw_readw(NS9750_BSD_PHYS_BASE + (off))
#define	ns_bsd_readl(off)	__raw_readl(NS9750_BSD_PHYS_BASE + (off))
#define	ns_bsd_writeb(val,off)	__raw_writeb(val, NS9750_BSD_PHYS_BASE + (off))
#define	ns_bsd_writew(val,off)	__raw_writew(val, NS9750_BSD_PHYS_BASE + (off))
#define	ns_bsd_writel(val,off)	__raw_writel(val, NS9750_BSD_PHYS_BASE + (off))
#define	ns_usb_readb(off)	__raw_readb(NS9750_USB_PHYS_BASE + (off))
#define	ns_usb_readw(off)	__raw_readw(NS9750_USB_PHYS_BASE + (off))
#define	ns_usb_readl(off)	__raw_readl(NS9750_USB_PHYS_BASE + (off))
#define	ns_usb_writeb(val,off)	__raw_writeb(val, NS9750_USB_PHYS_BASE + (off))
#define	ns_usb_writew(val,off)	__raw_writew(val, NS9750_USB_PHYS_BASE + (off))
#define	ns_usb_writel(val,off)	__raw_writel(val, NS9750_USB_PHYS_BASE + (off))
#define	ns_pci_readb(off)	__raw_readb(NS9750_PCI_PHYS_BASE + (off))
#define	ns_pci_readw(off)	__raw_readw(NS9750_PCI_PHYS_BASE + (off))
#define	ns_pci_readl(off)	__raw_readl(NS9750_PCI_PHYS_BASE + (off))
#define	ns_pci_writeb(val,off)	__raw_writeb(val, NS9750_PCI_PHYS_BASE + (off))
#define	ns_pci_writew(val,off)	__raw_writew(val, NS9750_PCI_PHYS_BASE + (off))
#define	ns_pci_writel(val,off)	__raw_writel(val, NS9750_PCI_PHYS_BASE + (off))
#endif

/*
 *  NS9750 internal registers
 */

/* System Configuration registers */
#define	NS_SCM_AHB_GEN_CONFIG		(0x0000) // AHB Arbiter Gen Configuration
#define	NS_SCM_BRC0			(0x0004) // BRC0
#define	NS_SCM_BRC1			(0x0008) // BRC1
#define	NS_SCM_BRC2			(0x000c) // BRC2
#define	NS_SCM_BRC3			(0x0010) // BRC3
#define	NS_SCM_AHB_BUS_ARBMON_TIMEOUT	(0x0014) // AHB Bus Arbiter/Monitor Timeout Period
#define	NS_SCM_AHB_ERR_DETECT_STATUS_1	(0x0018) // AHB Error Detect Status 1
#define	NS_SCM_AHB_ERR_DETECT_STATUS_2	(0x001c) // AHB Error Detect Status 2
#define	NS_SCM_AHB_ERR_MON_CONFIG	(0x0020) // AHB Error Monitoring Configuration
#define	NS_SCM_TMR0_RELOAD_COUNT	(0x0044) // Timer 0 Reload Count register
#define	NS_SCM_TMR1_RELDAD_COUNT	(0x0048) // Timer 1 Reload Count register
#define	NS_SCM_TMR2_RELOAD_COUNT	(0x004c) // Timer 2 Reload Count register
#define	NS_SCM_TMR3_RELOAD_COUNT	(0x0050) // Timer 3 Reload Count register
#define	NS_SCM_TMR4_RELOAD_COUNT	(0x0054) // Timer 4 Reload Count register
#define	NS_SCM_TMR5_RELOAD_COUNT	(0x0058) // Timer 5 Reload Count register
#define	NS_SCM_TMR6_RELOAD_COUNT	(0x005c) // Timer 6 Reload Count register
#define	NS_SCM_TMR7_RELOAD_COUNT	(0x0060) // Timer 7 Reload Count register
#define	NS_SCM_TMR8_RELOAD_COUNT	(0x0064) // Timer 8 Reload Count register
#define	NS_SCM_TMR9_RELOAD_COUNT	(0x0068) // Timer 9 Reload Count register
#define	NS_SCM_TMR10_RELOAD_COUNT	(0x006c) // Timer 10 Reload Count register
#define	NS_SCM_TMR11_RELOAD_COUNT	(0x0070) // Timer 11 Reload Count register
#define	NS_SCM_TMR12_RELOAD_COUNT	(0x0074) // Timer 12 Reload Count register
#define	NS_SCM_TMR13_RELOAD_COUNT	(0x0078) // Timer 13 Reload Count register
#define	NS_SCM_TMR14_RELOAD_COUNT	(0x007c) // Timer 14 Reload Count register
#define	NS_SCM_TMR15_RELOAD_COUNT	(0x0080) // Timer 15 Reload Count register
#define	NS_SCM_TMR0_READ		(0x0084) // Timer 0 Read register
#define	NS_SCM_TMR1_READ		(0x0088) // Timer 1 Read register
#define	NS_SCM_TMR2_READ		(0x008c) // Timer 2 Read register
#define	NS_SCM_TMR3_READ		(0x0090) // Timer 3 Read register
#define	NS_SCM_TMR4_READ		(0x0094) // Timer 4 Read register
#define	NS_SCM_TMR5_READ		(0x0098) // Timer 5 Read register
#define	NS_SCM_TMR6_READ		(0x009c) // Timer 6 Read register
#define	NS_SCM_TMR7_READ		(0x00a0) // Timer 7 Read register
#define	NS_SCM_TMR8_READ		(0x00a4) // Timer 8 Read register
#define	NS_SCM_TMR9_READ		(0x00a8) // Timer 9 Read register
#define	NS_SCM_TMR10_READ		(0x00ac) // Timer 10 Read register
#define	NS_SCM_TMR11_READ		(0x00b0) // Timer 11 Read register
#define	NS_SCM_TMR12_READ		(0x00b4) // Timer 12 Read register
#define	NS_SCM_TMR13_READ		(0x00b8) // Timer 13 Read register
#define	NS_SCM_TMR14_READ		(0x00bc) // Timer 14 Read register
#define	NS_SCM_TMR15_READ		(0x00c0) // Timer 15 Read register
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL0	(0x00c4) // Interrupt Vector Address Register Level 0
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL1	(0x00c8) // Interrupt Vector Address Register Level 1
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL2	(0x00cc) // Interrupt Vector Address Register Level 2
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL3	(0x00d0) // Interrupt Vector Address Register Level 3
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL4	(0x00d4) // Interrupt Vector Address Register Level 4
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL5	(0x00d8) // Interrupt Vector Address Register Level 5
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL6	(0x00dc) // Interrupt Vector Address Register Level 6
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL7	(0x00e0) // Interrupt Vector Address Register Level 7
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL8	(0x00e4) // Interrupt Vector Address Register Level 8
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL9	(0x00e8) // Interrupt Vector Address Register Level 9
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL10	(0x00ec) // Interrupt Vector Address Register Level 10
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL11	(0x00f0) // Interrupt Vector Address Register Level 11
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL12	(0x00f4) // Interrupt Vector Address Register Level 12
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL13	(0x00f8) // Interrupt Vector Address Register Level 13
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL14	(0x00fc) // Interrupt Vector Address Register Level 14
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL15	(0x0100) // Interrupt Vector Address Register Level 15
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL16	(0x0104) // Interrupt Vector Address Register Level 16
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL17	(0x0108) // Interrupt Vector Address Register Level 17
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL18	(0x010c) // Interrupt Vector Address Register Level 18
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL19	(0x0110) // Interrupt Vector Address Register Level 19
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL20	(0x0114) // Interrupt Vector Address Register Level 20
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL21	(0x0118) // Interrupt Vector Address Register Level 21
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL22	(0x011c) // Interrupt Vector Address Register Level 22
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL23	(0x0120) // Interrupt Vector Address Register Level 23
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL24	(0x0124) // Interrupt Vector Address Register Level 24
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL25	(0x0128) // Interrupt Vector Address Register Level 25
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL26	(0x012c) // Interrupt Vector Address Register Level 26
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL27	(0x0130) // Interrupt Vector Address Register Level 27
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL28	(0x0134) // Interrupt Vector Address Register Level 28
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL29	(0x0138) // Interrupt Vector Address Register Level 29
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL30	(0x013c) // Interrupt Vector Address Register Level 30
#define	NS_SCM_INT_VECTOR_ADDR_LEVEL31	(0x0140) // Interrupt Vector Address Register Level 31
#define	NS_SCM_INT_CONFIG_0_3		(0x0144) // Int Config 0-3
#define	NS_SCM_INT_CONFIG_4_7		(0x0148) // Int Config 4-7
#define	NS_SCM_INT_CONFIG_8_11		(0x014c) // Int Config 8-11
#define	NS_SCM_INT_CONFIG_12_15		(0x0150) // Int Config 12-15
#define	NS_SCM_INT_CONFIG_16_19		(0x0154) // Int Config 16-19
#define	NS_SCM_INT_CONFIG_20_23		(0x0158) // Int Config 20-23
#define	NS_SCM_INT_CONFIG_24_27		(0x015c) // Int Config 24-27
#define	NS_SCM_INT_CONFIG_28_31		(0x0160) // Int Config 28-31
#define	NS_SCM_ISRADDR			(0x0164) // ISRADDR
#define	NS_SCM_INT_STATUS_ACT		(0x0168) // Interrupt Status Active
#define	NS_SCM_INT_STATUS_RAW		(0x016c) // Interrupt Status Raw
#define	NS_SCM_TMR_INT_STATUS		(0x0170) // Timer Interrupt Status register
#define	NS_SCM_SW_WDOG_CONFIG		(0x0174) // Software Watchdog Configuration
#define	NS_SCM_SW_WDOG_TMR		(0x0178) // Software Watchdog Timer
#define	NS_SCM_CLOCK_CONFIG		(0x017c) // Clock Configuration register
#define	NS_SCM_RESET_SLEEP_CTRL		(0x0180) // Reset and Sleep Control register
#define	NS_SCM_MISC_SYS_CONFIG		(0x0184) // Miscellaneous System Configuration register
#define	NS_SCM_PLL_CONFIG		(0x0188) // PLL Configuration register
#define	NS_SCM_ACTIVE_INT_LEVEL		(0x018c) // Active Interrupt Level register
#define	NS_SCM_TMR0_CTRL		(0x0190) // Timer 0 Control register
#define	NS_SCM_TMR1_CTRL		(0x0194) // Timer 1 Control register
#define	NS_SCM_TMR2_CTRL		(0x0198) // Timer 2 Control register
#define	NS_SCM_TMR3_CTRL		(0x019c) // Timer 3 Control register
#define	NS_SCM_TMR4_CTRL		(0x01a0) // Timer 4 Control register
#define	NS_SCM_TMR5_CTRL		(0x01a4) // Timer 5 Control register
#define	NS_SCM_TMR6_CTRL		(0x01a8) // Timer 6 Control register
#define	NS_SCM_TMR7_CTRL		(0x01ac) // Timer 7 Control register
#define	NS_SCM_TMR8_CTRL		(0x01b0) // Timer 8 Control register
#define	NS_SCM_TMR9_CTRL		(0x01b4) // Timer 9 Control register
#define	NS_SCM_TMR10_CTRL		(0x01b8) // Timer 10 Control register
#define	NS_SCM_TMR11_CTRL		(0x01bc) // Timer 11 Control register
#define	NS_SCM_TMR12_CTRL		(0x01c0) // Timer 12 Control register
#define	NS_SCM_TMR13_CTRL		(0x01c4) // Timer 13 Control register
#define	NS_SCM_TMR14_CTRL		(0x01c8) // Timer 14 Control register
#define	NS_SCM_TMR15_CTRL		(0x01cc) // Timer 15 Control register
#define	NS_SCM_SYS_MEM_CS4_DYNAMIC_BASE	(0x01d0) // System Memory Chip Select 4 Dynamic Memory Base
#define	NS_SCM_SYS_MEM_CS4_DYNAMIC_MASK	(0x01d4) // System Memory Chip Select 4 Dynamic Memory Mask
#define	NS_SCM_SYS_MEM_CS5_DYNAMIC_BASE	(0x01d8) // System Memory Chip Select 5 Dynamic Memory Base
#define	NS_SCM_SYS_MEM_CS5_DYNAMIC_MASK	(0x01dc) // System Memory Chip Select 5 Dynamic Memory Mask
#define	NS_SCM_SYS_MEM_CS6_DYNAMIC_BASE	(0x01e0) // System Memory Chip Select 6 Dynamic Memory Base
#define	NS_SCM_SYS_MEM_CS6_DYNAMIC_MASK	(0x01e4) // System Memory Chip Select 6 Dynamic Memory Mask
#define	NS_SCM_SYS_MEM_CS7_DYNAMIC_BASE	(0x01e8) // System Memory Chip Select 7 Dynamic Memory Base
#define	NS_SCM_SYS_MEM_CS7_DYNAMIC_MASK	(0x01ec) // System Memory Chip Select 7 Dynamic Memory Mask
#define	NS_SCM_SYS_MEM_CS0_DYNAMIC_BASE	(0x01f0) // System Memory Chip Select 0 Dynamic Memory Base
#define	NS_SCM_SYS_MEM_CS0_DYNAMIC_MASK	(0x01f4) // System Memory Chip Select 0 Dynamic Memory Mask
#define	NS_SCM_SYS_MEM_CS1_DYNAMIC_BASE	(0x01f8) // System Memory Chip Select 1 Dynamic Memory Base
#define	NS_SCM_SYS_MEM_CS1_DYNAMIC_MASK	(0x01fc) // System Memory Chip Select 1 Dynamic Memory Mask
#define	NS_SCM_SYS_MEM_CS2_DYNAMIC_BASE	(0x0200) // System Memory Chip Select 2 Dynamic Memory Base
#define	NS_SCM_SYS_MEM_CS2_DYNAMIC_MASK	(0x0204) // System Memory Chip Select 2 Dynamic Memory Mask
#define	NS_SCM_SYS_MEM_CS3_DYNAMIC_BASE	(0x0208) // System Memory Chip Select 3 Dynamic Memory Base
#define	NS_SCM_SYS_MEM_CS3_DYNAMIC_MASK	(0x020c) // System Memory Chip Select 3 Dynamic Memory Mask
#define	NS_SCM_GEN_ID			(0x0210) // General purpose, user-defined ID register
#define	NS_SCM_EXTINT0_CTRL		(0x0214) // External Interrupt 0 Control register
#define	NS_SCM_EXTINT1_CTRL		(0x0218) // External Interrupt 1 Control register
#define	NS_SCM_EXTINT2_CTRL		(0x021c) // External Interrupt 2 Control register
#define	NS_SCM_EXTINT3_CTRL		(0x0220) // External Interrupt 3 Control register

/* Memory Control and Status registers */
#define	NS_MEM_CONTROL			(0x0000) // Control register
#define	NS_MEM_STATUS			(0x0004) // Status register
#define	NS_MEM_CONFIG			(0x0008) // Configuration register
#define	NS_MEM_DYNAMIC_CONTROL		(0x0020) // Dynamic Memory Control register
#define	NS_MEM_DYNAMIC_REFRESH		(0x0024) // Dynamic Memory Refresh Timer
#define	NS_MEM_DYNAMIC_READ_CONFIG	(0x0028) // Dynamic Memory Read Configuration register
#define	NS_MEM_DYNAMIC_TRP		(0x0030) // Dynamic Memory Precharge Command Period (tRP)
#define	NS_MEM_DYNAMIC_TRAS		(0x0034) // Dynamic Memory Active to Precharge Command Period (tRAS)
#define	NS_MEM_DYNAMIC_TSREX		(0x0038) // Dynamic Memory Self-Refresh Exit Time (tSREX)
#define	NS_MEM_DYNAMIC_TAPR		(0x003c) // Dynamic Memory Last Data Out to Active TIme (tARP)
#define	NS_MEM_DYNAMIC_TDAL		(0x0040) // Dynamic Memory Data-in to Active Command Time (tDAL or tAPW)
#define	NS_MEM_DYNAMIC_TWR		(0x0044) // Dynamic Memory Write Recovery Time (tWR, tDPL, tRWL, tRDL)
#define	NS_MEM_DYNAMIC_TRC		(0x0048) // Dynamic Memory Active to Active Command Period (tRC)
#define	NS_MEM_DYNAMIC_TRFC		(0x004c) // Dynamic Memory Auto Refresh Period, and Auto Refresh to Active Command Period (tRFC)
#define	NS_MEM_DYNAMIC_TXSR		(0x0050) // Dynamic Memory Exit Self-Refresh to Active Command (tXSR)
#define	NS_MEM_DYNAMIC_TRRD		(0x0054) // Dynamic Memory Active Bank A to Active B Time (tRRD)
#define	NS_MEM_DYNAMIC_TMRD		(0x0058) // Dynamic Memory Load Mode Register to Active Command Time (tMRD)
#define	NS_MEM_STATIC_EXTENDED_WAIT	(0x0080) // Static Memory Extended Wait
#define	NS_MEM_DYNAMIC_CONFIG_0		(0x0100) // Dynamic Memory Configuration Register 0
#define	NS_MEM_DYNAMIC_RAS_CAS_0	(0x0104) // Dynamic Memory RAS and CAS Delay 0
#define	NS_MEM_DYNAMIC_CONFIG_1		(0x0120) // Dynamic Memory Configuration Register 1
#define	NS_MEM_DYNAMIC_RAS_CAS_1	(0x0124) // Dynamic Memory RAS and CAS Delay 1
#define	NS_MEM_DYNAMIC_CONFIG_2		(0x0140) // Dynamic Memory Configuration Register 2
#define	NS_MEM_DYNAMIC_RAS_CAS_2	(0x0144) // Dynamic Memory RAS and CAS Delay 2
#define	NS_MEM_DYNAMIC_CONFIG_3		(0x0160) // Dynamic Memory Configuration Register 3
#define	NS_MEM_DYNAMIC_RAS_CAS_3	(0x0164) // Dynamic Memory RAS and CAS Delay 3
#define	NS_MEM_STATIC_CONFIG_0		(0x0200) // Static Memroy Configuration Register 0
#define	NS_MEM_STATIC_WAIT_WEN_0	(0x0204) // Static Memroy Write Enable Delay 0
#define	NS_MEM_STATIC_WAIT_OEN_0	(0x0208) // Static Memroy Output Enable Delay 0
#define	NS_MEM_STATIC_WAIT_RD_0		(0x020c) // Static Memroy Read Delay 0
#define	NS_MEM_STATIC_WAIT_PAGE_0	(0x0210) // Static Memroy Page Mode Read Delay 0
#define	NS_MEM_STATIC_WAIT_WR_0		(0x0214) // Static Memory Write Delay 0
#define	NS_MEM_STATIC_WAIT_TURN_0	(0x0218) // Static Memory Turn Round Delay 0
#define	NS_MEM_STATIC_CONFIG_1		(0x0220) // Static Memroy Configuration Register 1
#define	NS_MEM_STATIC_WAIT_WEN_1 	(0x0224) // Static Memroy Write Enable Delay 1
#define	NS_MEM_STATIC_WAIT_OEN_1 	(0x0228) // Static Memroy Output Enable Delay 1
#define	NS_MEM_STATIC_WAIT_RD_1		(0x022c) // Static Memroy Read Delay 1
#define	NS_MEM_STATIC_WAIT_PAGE_1	(0x0230) // Static Memroy Page Mode Read Delay 1
#define	NS_MEM_STATIC_WAIT_WR_1		(0x0234) // Static Memory Write Delay 1
#define	NS_MEM_STATIC_WAIT_TURN_1	(0x0238) // Static Memory Turn Round Delay 1
#define	NS_MEM_STATIC_CONFIG_2		(0x0240) // Static Memroy Configuration Register 2
#define	NS_MEM_STATIC_WAIT_WEN_2 	(0x0244) // Static Memroy Write Enable Delay 2
#define	NS_MEM_STATIC_WAIT_OEN_2 	(0x0248) // Static Memroy Output Enable Delay 2
#define	NS_MEM_STATIC_WAIT_RD_2		(0x024c) // Static Memroy Read Delay 2
#define	NS_MEM_STATIC_WAIT_PAGE_2	(0x0250) // Static Memroy Page Mode Read Delay 2
#define	NS_MEM_STATIC_WAIT_WR_2		(0x0254) // Static Memory Write Delay 2
#define	NS_MEM_STATIC_WAIT_TURN_2	(0x0258) // Static Memory Turn Round Delay 2
#define	NS_MEM_STATIC_CONFIG_3		(0x0260) // Static Memroy Configuration Register 3
#define	NS_MEM_STATIC_WAIT_WEN_3 	(0x0264) // Static Memroy Write Enable Delay 3
#define	NS_MEM_STATIC_WAIT_OEN_3 	(0x0268) // Static Memroy Output Enable Delay 3
#define	NS_MEM_STATIC_WAIT_RD_3		(0x026c) // Static Memroy Read Delay 3
#define	NS_MEM_STATIC_WAIT_PAGE_3	(0x0270) // Static Memroy Page Mode Read Delay 3
#define	NS_MEM_STATIC_WAIT_WR_3		(0x0274) // Static Memory Write Delay 3
#define	NS_MEM_STATIC_WAIT_TURN_3	(0x0278) // Static Memory Turn Round Delay 3

/* Ethernet Control and Status registers */
#define	NS_ETH_EGCR1			(0x0000) // Ethernet General Control Register #1
#define	NS_ETH_EGCR2			(0x0004) // Ethernet General Control Register #2
#define	NS_ETH_EGSR			(0x0008) // Ethernet General Status register
#define	NS_ETH_ETSR			(0x0018) // Ethernet Transmit Status register
#define	NS_ETH_ERSR			(0x001c) // Ethernet Receive Status register
#define	NS_ETH_MAC1			(0x0400) // MAC Configuration Register #1
#define	NS_ETH_MAC2			(0x0404) // MAC Configuration Register #2
#define	NS_ETH_IPGT			(0x0408) // Bac-to-Back Inter-Packet-Gap register
#define	NS_ETH_IPGR			(0x040c) // Non-Back-to-Back Inter-Packet-Gap register
#define	NS_ETH_CLRT			(0x0410) // Collision Window/Retry register
#define	NS_ETH_MAXF			(0x0414) // Maximum Frame register
#define	NS_ETH_SUPP			(0x0418) // PHY Support register
#define	NS_ETH_MCFG			(0x0420) // MII Management Configuration register
#define	NS_ETH_MCMD			(0x0424) // MII Management Command register
#define	NS_ETH_MADR			(0x0428) // MII Management Address register
#define	NS_ETH_MWTD			(0x042c) // MII Management Write Data register
#define	NS_ETH_MRDD			(0x0430) // MII Management Read Data register
#define	NS_ETH_MIND			(0x0434) // MII Management Indicators register
#define	NS_ETH_SA1			(0x0440) // Station Address Register #1
#define	NS_ETH_SA2			(0x0444) // Station Address Register #2
#define	NS_ETH_SA3			(0x0448) // Station Address Register #3
#define	NS_ETH_SAFR			(0x0500) // Station Address Filter register
#define	NS_ETH_HT1			(0x0504) // Hash Table Register #1
#define	NS_ETH_HT2			(0x0508) // Hash Table Register #2
#define	NS_ETH_STAT			(0x0680) // Statistics Register Base (45 registers)
#define	NS_ETH_RXAPTR			(0x0a00) // RX_A Buffer Descriptor Pointer register
#define	NS_ETH_RXBPTR			(0x0a04) // RX_B Buffer Descriptor Pointer register
#define	NS_ETH_RXCPTR			(0x0a08) // RX_C Buffer Descriptor Pointer register
#define	NS_ETH_RXDPTR			(0x0a0c) // RX_D Buffer Descriptor Pointer register
#define	NS_ETH_EINTR			(0x0a10) // Ethernet Interrupt Status register
#define	NS_ETH_EINTREN			(0x0a14) // Ethernet Interrupt Enable register
#define	NS_ETH_TXPTR			(0x0a18) // TX Buffer Descriptor Pointer register
#define	NS_ETH_TXRPTR			(0x0a1c) // TX Recover Buffer Descriptor Pointer register
#define	NS_ETH_TXERBD			(0x0a20) // TX Error Buffer Descriptor Pinter register
#define	NS_ETH_TXSPTR			(0x0a24) // TX Stall Buffer Descriptor Pinter register
#define	NS_ETH_RXAOFF			(0x0a28) // RX_A Buffer Descriptor Pointer Offset register
#define	NS_ETH_RXBOFF			(0x0a2c) // RX_B Buffer Descriptor Pointer Offset register
#define	NS_ETH_RXCOFF			(0x0a30) // RX_C Buffer Descriptor Pointer Offset register
#define	NS_ETH_RXDOFF			(0x0a34) // RX_D Buffer Descriptor Pointer Offset register
#define	NS_ETH_TXOFF 			(0x0a38) // Transmit Buffer Descriptor Pointer Offset register
#define	NS_ETH_RXFREE			(0x0a3c) // RX Free Buffer register
#define	NS_ETH_TXBD  			(0x1000) // TX Buffer Descriptor RAM (256 locations)

/* BBus Bridge Control and Status registers */
#define	NS_BBB_DMA1_BUF_DESCRIPTOR_PTR	(0x0000) // DMA1 Buffer Descriptor Pointer
#define	NS_BBB_DMA1_CTRL		(0x0004) // DMA1 Control register
#define	NS_BBB_DMA1_STATUS_INT_ENABLE	(0x0008) // DMA1 Status and Interrupt Enable
#define	NS_BBB_DMA1_PCS			(0x000c) // DMA1 Peripheral Chip Select
#define	NS_BBB_DMA2_BUF_DESCRIPTOR_PTR	(0x0020) // DMA2 Buffer Descriptor Pointer
#define	NS_BBB_DMA2_CTRL		(0x0024) // DMA2 Control register
#define	NS_BBB_DMA2_STATUS_INT_ENABLE	(0x0028) // DMA2 Status and Interrupt Enable
#define	NS_BBB_DMA2_PERIPHERAL_CS	(0x002c) // DMA2 Peripheral Chip Select
#define	NS_BBB_BBUS_BRIDGE_INT_STATUS	(0x1000) // BBus Bridge Interrupt Status
#define	NS_BBB_BBUS_BRIDGE_INT_ENABLE	(0x1004) // BBus Bridge Interrupt Enable

/* DMA Control and Status registers */
#define	NS_DMA1_CH1_BUF_DESCRIPTOR_PTR	(0x0000) // DMA1 Channel 1 Buffer Descriptor Pointer
#define	NS_DMA1_CH1_CTRL		(0x0010) // DMA1 Channel 1 Control register
#define	NS_DMA1_CH1_STATUS_INT_ENABLE	(0x0014) // DMA1 Channel 1 Status/Interrupt Enable register
#define	NS_DMA1_CH2_BUF_DESCRIPTOR_PTR	(0x0020) // DMA1 Channel 2 Buffer Descriptor Pointer
#define	NS_DMA1_CH2_CTRL		(0x0030) // DMA1 Channel 2 Control register
#define	NS_DMA1_CH2_STATUS_INT_ENABLE	(0x0034) // DMA1 Channel 2 Status/Interrupt Enable register
#define	NS_DMA1_CH3_BUF_DESCRIPTOR_PTR	(0x0040) // DMA1 Channel 3 Buffer Descriptor Pointer
#define	NS_DMA1_CH3_CTRL		(0x0050) // DMA1 Channel 3 Control register
#define	NS_DMA1_CH3_STATUS_INT_ENABLE	(0x0054) // DMA1 Channel 3 Status/Interrupt Enable register
#define	NS_DMA1_CH4_BUF_DESCRIPTOR_PTR	(0x0060) // DMA1 Channel 4 Buffer Descriptor Pointer
#define	NS_DMA1_CH4_CTRL		(0x0070) // DMA1 Channel 4 Control register
#define	NS_DMA1_CH4_STATUS_INT_ENABLE	(0x0074) // DMA1 Channel 4 Status/Interrupt Enable register
#define	NS_DMA1_CH5_BUF_DESCRIPTOR_PTR	(0x0080) // DMA1 Channel 5 Buffer Descriptor Pointer
#define	NS_DMA1_CH5_CTRL		(0x0090) // DMA1 Channel 5 Control register
#define	NS_DMA1_CH5_STATUS_INT_ENABLE	(0x0094) // DMA1 Channel 5 Status/Interrupt Enable register
#define	NS_DMA1_CH6_BUF_DESCRIPTOR_PTR	(0x00a0) // DMA1 Channel 6 Buffer Descriptor Pointer
#define	NS_DMA1_CH6_CTRL		(0x00b0) // DMA1 Channel 6 Control register
#define	NS_DMA1_CH6_STATUS_INT_ENABLE	(0x00b4) // DMA1 Channel 6 Status/Interrupt Enable register
#define	NS_DMA1_CH7_BUF_DESCRIPTOR_PTR	(0x00c0) // DMA1 Channel 7 Buffer Descriptor Pointer
#define	NS_DMA1_CH7_CTRL		(0x00d0) // DMA1 Channel 7 Control register
#define	NS_DMA1_CH7_STATUS_INT_ENABLE	(0x00d4) // DMA1 Channel 7 Status/Interrupt Enable register
#define	NS_DMA1_CH8_BUF_DESCRIPTOR_PTR	(0x00e0) // DMA1 Channel 8 Buffer Descriptor Pointer
#define	NS_DMA1_CH8_CTRL		(0x00f0) // DMA1 Channel 8 Control register
#define	NS_DMA1_CH8_STATUS_INT_ENABLE	(0x00f4) // DMA1 Channel 8 Status/Interrupt Enable register
#define	NS_DMA1_CH9_BUF_DESCRIPTOR_PTR	(0x0100) // DMA1 Channel 9 Buffer Descriptor Pointer
#define	NS_DMA1_CH9_CTRL		(0x0110) // DMA1 Channel 9 Control register
#define	NS_DMA1_CH9_STATUS_INT_ENABLE	(0x0114) // DMA1 Channel 9 Status/Interrupt Enable register
#define	NS_DMA1_CH10_BUF_DESCRIPTOR_PTR	(0x0120) // DMA1 Channel 10 Buffer Descriptor Pointer
#define	NS_DMA1_CH10_CTRL		(0x0130) // DMA1 Channel 10 Control register
#define	NS_DMA1_CH10_STATUS_INT_ENABLE	(0x0134) // DMA1 Channel 10 Status/Interrupt Enable register
#define	NS_DMA1_CH11_BUF_DESCRIPTOR_PTR	(0x0140) // DMA1 Channel 11 Buffer Descriptor Pointer
#define	NS_DMA1_CH11_CTRL		(0x0150) // DMA1 Channel 11 Control register
#define	NS_DMA1_CH11_STATUS_INT_ENABLE	(0x0154) // DMA1 Channel 11 Status/Interrupt Enable register
#define	NS_DMA1_CH12_BUF_DESCRIPTOR_PTR	(0x0160) // DMA1 Channel 12 Buffer Descriptor Pointer
#define	NS_DMA1_CH12_CTRL		(0x0170) // DMA1 Channel 12 Control register
#define	NS_DMA1_CH12_STATUS_INT_ENABLE	(0x0174) // DMA1 Channel 12 Status/Interrupt Enable register
#define	NS_DMA1_CH13_BUF_DESCRIPTOR_PTR	(0x0180) // DMA1 Channel 13 Buffer Descriptor Pointer
#define	NS_DMA1_CH13_CTRL		(0x0190) // DMA1 Channel 13 Control register
#define	NS_DMA1_CH13_STATUS_INT_ENABLE	(0x0194) // DMA1 Channel 13 Status/Interrupt Enable register
#define	NS_DMA1_CH14_BUF_DESCRIPTOR_PTR	(0x01a0) // DMA1 Channel 14 Buffer Descriptor Pointer
#define	NS_DMA1_CH14_CTRL		(0x01b0) // DMA1 Channel 14 Control register
#define	NS_DMA1_CH14_STATUS_INT_ENABLE	(0x01b4) // DMA1 Channel 14 Status/Interrupt Enable register
#define	NS_DMA1_CH15_BUF_DESCRIPTOR_PTR	(0x01c0) // DMA1 Channel 15 Buffer Descriptor Pointer
#define	NS_DMA1_CH15_CTRL		(0x01d0) // DMA1 Channel 15 Control register
#define	NS_DMA1_CH15_STATUS_INT_ENABLE	(0x01d4) // DMA1 Channel 15 Status/Interrupt Enable register
#define	NS_DMA1_CH16_BUF_DESCRIPTOR_PTR	(0x01e0) // DMA1 Channel 16 Buffer Descriptor Pointer
#define	NS_DMA1_CH16_CTRL		(0x01f0) // DMA1 Channel 16 Control register
#define	NS_DMA1_CH16_STATUS_INT_ENABLE	(0x01f4) // DMA1 Channel 16 Status/Interrupt Enable register
#define	NS_DMA2_CH1_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH1_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH1_CTRL		(0x110000+NS_DMA1_CH1_CTRL)
#define	NS_DMA2_CH1_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH1_STATUS_INT_ENABLE)
#define	NS_DMA2_CH2_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH2_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH2_CTRL		(0x110000+NS_DMA1_CH2_CTRL)
#define	NS_DMA2_CH2_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH2_STATUS_INT_ENABLE)
#define	NS_DMA2_CH3_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH3_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH3_CTRL		(0x110000+NS_DMA1_CH3_CTRL)
#define	NS_DMA2_CH3_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH3_STATUS_INT_ENABLE)
#define	NS_DMA2_CH4_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH4_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH4_CTRL		(0x110000+NS_DMA1_CH4_CTRL)
#define	NS_DMA2_CH4_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH4_STATUS_INT_ENABLE)
#define	NS_DMA2_CH5_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH5_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH5_CTRL		(0x110000+NS_DMA1_CH5_CTRL)
#define	NS_DMA2_CH5_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH5_STATUS_INT_ENABLE)
#define	NS_DMA2_CH6_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH6_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH6_CTRL		(0x110000+NS_DMA1_CH6_CTRL)
#define	NS_DMA2_CH6_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH6_STATUS_INT_ENABLE)
#define	NS_DMA2_CH7_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH7_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH7_CTRL		(0x110000+NS_DMA1_CH7_CTRL)
#define	NS_DMA2_CH7_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH7_STATUS_INT_ENABLE)
#define	NS_DMA2_CH8_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH8_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH8_CTRL		(0x110000+NS_DMA1_CH8_CTRL)
#define	NS_DMA2_CH8_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH8_STATUS_INT_ENABLE)
#define	NS_DMA2_CH9_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH9_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH9_CTRL		(0x110000+NS_DMA1_CH9_CTRL)
#define	NS_DMA2_CH9_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH9_STATUS_INT_ENABLE)
#define	NS_DMA2_CH10_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH10_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH10_CTRL		(0x110000+NS_DMA1_CH10_CTRL)
#define	NS_DMA2_CH10_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH10_STATUS_INT_ENABLE)
#define	NS_DMA2_CH11_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH11_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH11_CTRL		(0x110000+NS_DMA1_CH11_CTRL)
#define	NS_DMA2_CH11_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH11_STATUS_INT_ENABLE)
#define	NS_DMA2_CH12_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH12_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH12_CTRL		(0x110000+NS_DMA1_CH12_CTRL)
#define	NS_DMA2_CH12_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH12_STATUS_INT_ENABLE)
#define	NS_DMA2_CH13_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH13_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH13_CTRL		(0x110000+NS_DMA1_CH13_CTRL)
#define	NS_DMA2_CH13_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH13_STATUS_INT_ENABLE)
#define	NS_DMA2_CH14_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH14_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH14_CTRL		(0x110000+NS_DMA1_CH14_CTRL)
#define	NS_DMA2_CH14_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH14_STATUS_INT_ENABLE)
#define	NS_DMA2_CH15_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH15_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH15_CTRL		(0x110000+NS_DMA1_CH15_CTRL)
#define	NS_DMA2_CH15_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH15_STATUS_INT_ENABLE)
#define	NS_DMA2_CH16_BUF_DESCRIPTOR_PTR	(0x110000+NS_DMA1_CH16_BUF_DESCRIPTOR_PTR)
#define	NS_DMA2_CH16_CTRL		(0x110000+NS_DMA1_CH16_CTRL)
#define	NS_DMA2_CH16_STATUS_INT_ENABLE	(0x110000+NS_DMA1_CH16_STATUS_INT_ENABLE)

/* BBus Utility Control and Status registers */
#define	NS_BBU_MASTER_RESET		(0x0000) // Master Reset register
#define	NS_BBU_GPIO_CONFIG_1		(0x0010) // GPIO Configuration Register #1
#define	NS_BBU_GPIO_CONFIG_2		(0x0014) // GPIO Configuration Register #2
#define	NS_BBU_GPIO_CONFIG_3		(0x0018) // GPIO Configuration Register #3
#define	NS_BBU_GPIO_CONFIG_4		(0x001c) // GPIO Configuration Register #4
#define	NS_BBU_GPIO_CONFIG_5		(0x0020) // GPIO Configuration Register #5
#define	NS_BBU_GPIO_CONFIG_6		(0x0024) // GPIO Configuration Register #6
#define	NS_BBU_GPIO_CONFIG_7		(0x0028) // GPIO Configuration Register #7
#define	NS_BBU_GPIO_CTRL_1		(0x0030) // GPIO Control Register #1
#define	NS_BBU_GPIO_CTRL_2		(0x0034) // GPIO Control Register #2
#define	NS_BBU_GPIO_STATUS_1		(0x0040) // GPIO Status Register #1
#define	NS_BBU_GPIO_STATUS_2		(0x0044) // GPIO Status Register #2
#define	NS_BBU_BBUS_MONITOR		(0x0050) // BBus Monitor register
#define	NS_BBU_BBUS_DMAINT_STATUS	(0x0060) // BBus DMA Interrupt Status register
#define	NS_BBU_BBUS_DMAINT_ENABLE	(0x0064) // BBus DMA Interrupt Enable register
#define	NS_BBU_USB_CONFIG		(0x0070) // USB Configuration register
#define	NS_BBU_ENDIAN_CONFIG		(0x0080) // Endian Configuration register
#define	NS_BBU_ARM_WAKEUP		(0x0090) // ARM Wake-up register

/* I2C registers */
#define	NS_I2C_CMD_TX_DATA		(0x0000) // Command Transmit Data register
#define	NS_I2C_STATUS_RX_DATA		(0x0000) // Status Receive Data register
#define	NS_I2C_MASTER_ADDR		(0x0004) // Master Address register
#define	NS_I2C_SLAVE_ADDR		(0x0008) // Slave Address register
#define	NS_I2C_CONFIG			(0x000c) // Configuration register

/* LCD Control and Status registers */
#define	NS_LCD_TIMING0			(0x0000) // Horizontal axis panel control
#define	NS_LCD_TIMING1			(0x0004) // Vertical axis panel control
#define	NS_LCD_TIMING2			(0x0008) // Clock and signal polarity control
#define	NS_LCD_TIMING3			(0x000c) // Line end control
#define	NS_LCD_UPBASE			(0x0010) // Upper panel frame base address
#define	NS_LCD_LPBASE			(0x0014) // Lower panel frame base address
#define	NS_LCD_INTRENABLE		(0x0018) // Interrupt enable mask
#define	NS_LCD_CONTROL			(0x001c) // LCD panel pixel parameters
#define	NS_LCD_STATUS			(0x0020) // Raw interrupt status
#define	NS_LCD_INTERRUPT		(0x0024) // Final masked interrupts
#define	NS_LCD_UPCURR			(0x0028) // LCD upper panel current address value
#define	NS_LCD_LPCURR			(0x002c) // LCD lower panel current address value
#define	NS_LCD_PALETTE			(0x0200) // 256 x 16-bit calor palette

/* Serial Port Control and Status registers */
#define	NS_SER_CHB_CTRL_A		(0x0000) // Channel B Control Register A
#define	NS_SER_CHB_CTRL_B		(0x0004) // Channel B Control Register B
#define	NS_SER_CHB_STATUS_A		(0x0008) // Channel B Status Register A
#define 	SER_STATUS_RXFDB_MASK		(0x00300000)
#define 	SER_STATUS_RXFDB_SHIFT		(20)
#define 	SER_STATUS_RXFDB(status)	(((status) & \
			SER_STATUS_RXFDB_MASK) >> SER_STATUS_RXFDB_SHIFT)
#define		SER_STATUS_RRDY			(0x00000800)
#define		SER_STATUS_RHALF		(0x00000400)
#define 	SER_STATUS_RBC			(0x00000200)
#define 	SER_STATUS_RFS			(0x00000100)
#define 	SER_STATUS_TRDY			(0x00000008)
#define 	SER_STATUS_THALF		(0x00000004)
#define 	SER_STATUS_TBC			(0x00000002)
#define 	SER_STATUS_TEMPTY		(0x00000001)
#define	NS_SER_CHB_BITRATE		(0x000c) // Channel B Bit-Rate register
#define	NS_SER_CHB_FIFO_DATA		(0x0010) // Channel B FIFO Data register
#define	NS_SER_CHB_RX_BUF_TMR		(0x0014) // Channel B Receive Buffer Gap Timer (UART/SPI)
#define	NS_SER_CHB_HDLC_MAX		(0x0014) // Channel B HDLC Max Length register (HDLC)
#define	NS_SER_CHB_RX_CHAR_TMR		(0x0018) // Channel B Receive Character Gap Timer
#define	NS_SER_CHB_RX_MATCH		(0x001c) // Channel B Receive Match register
#define	NS_SER_CHB_RX_MATCH_MASK	(0x0020) // Channel B Receive Match Mask register
#define	NS_SER_CHB_CTRL_C		(0x0024) // Channel B Control Register C (HDLC)
#define	NS_SER_CHB_STATUS_B		(0x0028) // Channel B Status Register B (HDLC)
#define	NS_SER_CHB_STATUS_C		(0x002c) // Channel B Status Register C (HDLC)
#define	NS_SER_CHB_FIFO_DATA_LAST	(0x0030) // Channel B FIFO Data Last register (HDLC)
#define	NS_SER_CHB_FLOW_CTRL		(0x0034) // Channel B Flow Control register (UART)
#define	NS_SER_CHB_FLOW_CTRL_FORCE	(0x0038) // Channel B Flow Control Force register (UART/SPI)
#define	NS_SER_CHA_CTRL_A		(0x40+NS_SER_CHB_CTRL_A)
#define	NS_SER_CHA_CTRL_B		(0x40+NS_SER_CHB_CTRL_B)
#define	NS_SER_CHA_STATUS_A		(0x40+NS_SER_CHB_STATUS_A)
#define	NS_SER_CHA_BITRATE		(0x40+NS_SER_CHB_BITRATE)
#define	NS_SER_CHA_FIFO_DATA		(0x40+NS_SER_CHB_FIFO_DATA)
#define	NS_SER_CHA_RX_BUF_TMR		(0x40+NS_SER_CHB_RX_BUF_TMR)
#define	NS_SER_CHA_HDLC_MAX		(0x40+NS_SER_CHB_HDLC_MAX)
#define	NS_SER_CHA_RX_CHAR_TMR		(0x40+NS_SER_CHB_RX_CHAR_TMR)
#define	NS_SER_CHA_RX_MATCH		(0x40+NS_SER_CHB_RX_MATCH)
#define	NS_SER_CHA_RX_MATCH_MASK	(0x40+NS_SER_CHB_RX_MATCH_MASK)
#define	NS_SER_CHA_CTRL_C		(0x40+NS_SER_CHB_CTRL_C)
#define	NS_SER_CHA_STATUS_B		(0x40+NS_SER_CHB_STATUS_B)
#define	NS_SER_CHA_STATUS_C		(0x40+NS_SER_CHB_STATUS_C)
#define	NS_SER_CHA_FIFO_DATA_LAST	(0x40+NS_SER_CHB_FIFO_LAST_DATA)
#define	NS_SER_CHA_FLOW_CTRL		(0x40+NS_SER_CHB_FLOW_CTRL)
#define	NS_SER_CHA_FLOW_CTRL_FORCE	(0x40+NS_SER_CHB_FLOW_CTRL_FORCE)
#define	NS_SER_CHC_CTRL_A		(0x100000+NS_SER_CHB_CTRL_A)
#define	NS_SER_CHC_CTRL_B		(0x100000+NS_SER_CHB_CTRL_B)
#define	NS_SER_CHC_STATUS_A		(0x100000+NS_SER_CHB_STATUS_A)
#define	NS_SER_CHC_BITRATE		(0x100000+NS_SER_CHB_BITRATE)
#define	NS_SER_CHC_FIFO_DATA		(0x100000+NS_SER_CHB_FIFO_DATA)
#define	NS_SER_CHC_RX_BUF_TMR		(0x100000+NS_SER_CHB_RX_BUF_TMR)
#define	NS_SER_CHC_HDLC_MAX		(0x100000+NS_SER_CHB_HDLC_MAX)
#define	NS_SER_CHC_RX_CHAR_TMR		(0x100000+NS_SER_CHB_RX_CHAR_TMR)
#define	NS_SER_CHC_RX_MATCH		(0x100000+NS_SER_CHB_RX_MATCH)
#define	NS_SER_CHC_RX_MATCH_MASK	(0x100000+NS_SER_CHB_RX_MATCH_MASK)
#define	NS_SER_CHC_CTRL_C		(0x100000+NS_SER_CHB_CTRL_C)
#define	NS_SER_CHC_STATUS_B		(0x100000+NS_SER_CHB_STATUS_B)
#define	NS_SER_CHC_STATUS_C		(0x100000+NS_SER_CHB_STATUS_C)
#define	NS_SER_CHC_FIFO_DATA_LAST	(0x100000+NS_SER_CHB_FIFO_DATA_LAST)
#define	NS_SER_CHC_FLOW_CTRL		(0x100000+NS_SER_CHB_FLOW_CTRL)
#define	NS_SER_CHC_FLOW_CTRL_FORCE	(0x100000+NS_SER_CHB_FLOW_CTRL_FORCE)
#define	NS_SER_CHD_CTRL_A		(0x100000+NS_SER_CHA_CTRL_A)
#define	NS_SER_CHD_CTRL_B		(0x100000+NS_SER_CHA_CTRL_B)
#define	NS_SER_CHD_STATUS_A		(0x100000+NS_SER_CHA_STATUS_A)
#define	NS_SER_CHD_BITRATE		(0x100000+NS_SER_CHA_BITRATE)
#define	NS_SER_CHD_FIFO_DATA		(0x100000+NS_SER_CHA_FIFO_DATA)
#define	NS_SER_CHD_RX_BUF_TMR		(0x100000+NS_SER_CHA_RX_BUF_TMR)
#define	NS_SER_CHD_HDLC_MAX		(0x100000+NS_SER_CHA_HDLC_MAX)
#define	NS_SER_CHD_RX_CHAR_TMR		(0x100000+NS_SER_CHA_RX_CHAR_TMR)
#define	NS_SER_CHD_RX_MATCH		(0x100000+NS_SER_CHA_RX_MATCH)
#define	NS_SER_CHD_RX_MATCH_MASK	(0x100000+NS_SER_CHA_RX_MATCH_MASK)
#define	NS_SER_CHD_CTRL_C		(0x100000+NS_SER_CHA_CTRL_C)
#define	NS_SER_CHD_STATUS_B		(0x100000+NS_SER_CHA_STATUS_B)
#define	NS_SER_CHD_STATUS_C		(0x100000+NS_SER_CHA_STATUS_C)
#define	NS_SER_CHD_FIFO_DATA_LAST	(0x100000+NS_SER_CHA_FIFO_DATA_LAST)
#define	NS_SER_CHD_FLOW_CTRL		(0x100000+NS_SER_CHA_FLOW_CTRL)
#define	NS_SER_CHD_FLOW_CTRL_FORCE	(0x100000+NS_SER_CHA_FLOW_CTRL_FORCE)

/* BBus Slave and DMA Interface registers */
#define	NS_BSD_GEN_CONFIG		(0x0000) // Generarl Configuration register
#define	NS_BSD_INTERRUPT_STATUS		(0x0004) // Interrupt Status and Control register
#define	NS_BSD_INTERRUPT_CONTROL	(NS_BSD_INTERRUPT_STATUS)
#define	NS_BSD_FIFO_STATUS		(0x0008) // FIFO Status register
#define	NS_BSD_FW_CMD_FIFO_READ_REG	(0x000c) // Forward Command FIFO Read register
#define	NS_BSD_FW_DAT_FIFO_READ_REG	(0x0010) // Forward Data FIFO Read register
#define	NS_BSD_RV_FIFO_WRITE_REG	(0x001c) // Reverse FIFO Write register
#define	NS_BSD_RV_FIFO_WRITE_REG_LAST	(0x0020) // Reverse FIFO Write register - Last
#define	NS_BSD_FW_CMD_DMA_CONTROL	(0x0024) // Forward Command DMA Control register
#define	NS_BSD_FW_DAT_DMA_CONTROL	(0x0028) // Forward Data DMA Control register
#define	NS_BSD_PD			(0x0100) // Printer Data Pins register
#define	NS_BSD_PSR			(0x0104) // Port Status register (host)
#define	NS_BSD_PCR			(0x0108) // Port Control register
#define	NS_BSD_PIN			(0x010c) // Port Status register (peripheral)
#define	NS_BSD_FEA			(0x0114) // Feature Control Register A
#define	NS_BSD_FEB			(0x0118) // Feature Control Register B
#define	NS_BSD_FEI			(0x011c) // Interrupt Enable register
#define	NS_BSD_FEM			(0x0120) // Master Enable register
#define	NS_BSD_EXR			(0x0124) // Extensibility Byte Requested by Master
#define	NS_BSD_ECR			(0x0128) // Extended Control register
#define	NS_BSD_STI			(0x012c) // Interrupt Status register
#define	NS_BSD_MSK			(0x0134) // Mask Pin Interrupt
#define	NS_BSD_PIT			(0x0138) // Pin Interrupt Control
#define	NS_BSD_GRN			(0x0168) // Granularity Value
#define	NS_BSD_ECA			(0x0174) // Forward Address register
#define	NS_BSD_PHA			(0x0178) // Core Phase register

/* USB Block registers */
#define	NS_USB_GLOBAL_CTRL		(0x0000) // USB Global Control/Status register
#define	NS_USB_GLOBAL_STATUS		(USB_GLOBAL_CTRL)
#define	NS_USB_DEVICE_CTRL		(0x0004) // USB Device Control/Status register
#define	NS_USB_DEVICE_STATUS		(USB_DEVICE_CTRL)
#define	NS_USB_GLOBAL_INT_ENABLE	(0x000c) // Global Interrupt Enable
#define	NS_USB_GLOBAL_INT_STATUS	(0x0010) // Global Interrupt Status
#define	NS_USB_DEVICE_IP_PROG_CTRL	(0x0014) // Device IP Programming Control/Status
#define	NS_USB_DEVICE_IP_PROG_STATUS	(USB_DEVICE_IP_PROG_CTRL)
#define	NS_USB_HC_REVISION		(0x1000) // HcRevision register
#define	NS_USB_HC_CONTROL		(0x1004) // HcControl register
#define	NS_USB_HC_COMMAND_STATUS	(0x1008) // HcCommandStatus register
#define	NS_USB_HC_INTERRUPT_STATUS	(0x100c) // HcInterruptStatus register
#define	NS_USB_HC_INTERRUPT_ENABLE	(0x1010) // HcInterruptEnable register
#define	NS_USB_HC_INTERRUPT_DISABLE	(0x1014) // HcInterrupt Disable register
#define	NS_USB_HC_HCCA			(0x1018) // HcHCCA (Host Control Commnunications Area) register
#define	NS_USB_HC_PERIOD_CURRENT_ED	(0x101c) // HcPeriodCurrentED (Endpoint Descriptor) register
#define	NS_USB_HC_CONTROL_HEAD_ED	(0x1020) // HcControlHeadED register
#define	NS_USB_HC_CONTROL_CURRENT_ED	(0x1024) // HcControlCurrentED register
#define	NS_USB_HC_BULK_HEAD_ED		(0x1028) // HcBulkHeadED register
#define	NS_USB_HC_BULK_CURRENT_ED	(0x102c) // HcBulkCurrentED register
#define	NS_USB_HC_DONE_HEAD		(0x1030) // HcDoneHead register
#define	NS_USB_HC_FM_INTERVAL		(0x1034) // HcFmInterval register (Fm = Frame)
#define	NS_USB_HC_FM_REMAINING		(0x1038) // HcFmRemaining register
#define	NS_USB_HC_FM_NUMBER		(0x103c) // HcFmNumber register
#define	NS_USB_HC_PERIODIC_START	(0x1040) // HcPeriodicStart register
#define	NS_USB_HC_LS_THRESHOLD		(0x1044) // HcLsThreshold register
#define	NS_USB_HC_RH_DESCRIPTOR_A	(0x1048) // HcRhDescriptorA register (Rh = Root hub)
#define	NS_USB_HC_RH_DESCRIPTOR_B	(0x104c) // HcRhDescriptorB register
#define	NS_USB_HC_RH_STATUS		(0x1050) // HcRhStatus register
#define	NS_USB_HC_RH_PORT_STATUS_1	(0x1054) // HcRhPortStatus[1] register
#define	NS_USB_ENDPOINT_DESCRIPTOR_0	(0x2004) // Endpoint Descriptor #0
#define	NS_USB_ENDPOINT_DESCRIPTOR_1	(0x2008) // Endpoint Descriptor #1
#define	NS_USB_ENDPOINT_DESCRIPTOR_2	(0x200c) // Endpoint Descriptor #2
#define	NS_USB_ENDPOINT_DESCRIPTOR_3	(0x2010) // Endpoint Descriptor #3
#define	NS_USB_ENDPOINT_DESCRIPTOR_4	(0x2014) // Endpoint Descriptor #4
#define	NS_USB_ENDPOINT_DESCRIPTOR_5	(0x2018) // Endpoint Descriptor #5
#define	NS_USB_ENDPOINT_DESCRIPTOR_6	(0x201c) // Endpoint Descriptor #6
#define	NS_USB_ENDPOINT_DESCRIPTOR_7	(0x2020) // Endpoint Descriptor #7
#define	NS_USB_ENDPOINT_DESCRIPTOR_8	(0x2024) // Endpoint Descriptor #8
#define	NS_USB_ENDPOINT_DESCRIPTOR_9	(0x2028) // Endpoint Descriptor #9
#define	NS_USB_ENDPOINT_DESCRIPTOR_10	(0x202c) // Endpoint Descriptor #10
#define	NS_USB_ENDPOINT_DESCRIPTOR_11	(0x2030) // Endpoint Descriptor #11
#define	NS_USB_FIFO_INTERRUPT_STATUS_0	(0x3000) // FIFO Interrupt Status 0
#define	NS_USB_FIFO_INTERRUPT_ENABLE_0	(0x3004) // FIFO Interrupt Enable 0
#define	NS_USB_FIFO_INTERRUPT_STATUS_1	(0x3010) // FIFO Interrupt Status 1
#define	NS_USB_FIFO_INTERRUPT_ENABLE_1	(0x3014) // FIFO Interrupt Enable 1
#define	NS_USB_FIFO_INTERRUPT_STATUS_2	(0x3020) // FIFO Interrupt Status 2
#define	NS_USB_FIFO_INTERRUPT_ENABLE_2	(0x3024) // FIFO Interrupt Enable 2
#define	NS_USB_FIFO_INTERRUPT_STATUS_3	(0x3030) // FIFO Interrupt Status 3
#define	NS_USB_FIFO_INTERRUPT_ENABLE_3	(0x3034) // FIFO Interrupt Enable 3
#define	NS_USB_FIFO_PACKET_CONTROL_1	(0x3080) // FIFO Packet Control #1
#define	NS_USB_FIFO_PACKET_CONTROL_2	(0x3084) // FIFO Packet Control #2
#define	NS_USB_FIFO_PACKET_CONTROL_3	(0x3088) // FIFO Packet Control #3
#define	NS_USB_FIFO_PACKET_CONTROL_4	(0x308c) // FIFO Packet Control #4
#define	NS_USB_FIFO_PACKET_CONTROL_5	(0x3090) // FIFO Packet Control #5
#define	NS_USB_FIFO_PACKET_CONTROL_6	(0x3094) // FIFO Packet Control #6
#define	NS_USB_FIFO_PACKET_CONTROL_7	(0x3098) // FIFO Packet Control #7
#define	NS_USB_FIFO_PACKET_CONTROL_8	(0x309c) // FIFO Packet Control #8
#define	NS_USB_FIFO_PACKET_CONTROL_9	(0x30a0) // FIFO Packet Control #9
#define	NS_USB_FIFO_PACKET_CONTROL_10	(0x30a4) // FIFO Packet Control #10
#define	NS_USB_FIFO_PACKET_CONTROL_11	(0x30a8) // FIFO Packet Control #11
#define	NS_USB_FIFO_PACKET_CONTROL_12	(0x30ac) // FIFO Packet Control #12
#define	NS_USB_FIFO_PACKET_CONTROL_13	(0x30b0) // FIFO Packet Control #13
#define	NS_USB_FIFO_STATUS_1		(0x3100) // FIFO Status and Control #1
#define	NS_USB_FIFO_CONTROL_1		(NS_USB_FIFO_STATUS_1)
#define	NS_USB_FIFO_STATUS_2		(0x3108) // FIFO Status and Control #2
#define	NS_USB_FIFO_CONTROL_2		(NS_USB_FIFO_STATUS_2)
#define	NS_USB_FIFO_STATUS_3		(0x3110) // FIFO Status and Control #3
#define	NS_USB_FIFO_CONTROL_3		(NS_USB_FIFO_STATUS_3)
#define	NS_USB_FIFO_STATUS_4		(0x3118) // FIFO Status and Control #4
#define	NS_USB_FIFO_CONTROL_4		(NS_USB_FIFO_STATUS_4)
#define	NS_USB_FIFO_STATUS_5		(0x3120) // FIFO Status and Control #5
#define	NS_USB_FIFO_CONTROL_5		(NS_USB_FIFO_STATUS_5)
#define	NS_USB_FIFO_STATUS_6		(0x3128) // FIFO Status and Control #6
#define	NS_USB_FIFO_CONTROL_6		(NS_USB_FIFO_STATUS_6)
#define	NS_USB_FIFO_STATUS_7		(0x3130) // FIFO Status and Control #7
#define	NS_USB_FIFO_CONTROL_7		(NS_USB_FIFO_STATUS_7)
#define	NS_USB_FIFO_STATUS_8		(0x3138) // FIFO Status and Control #8
#define	NS_USB_FIFO_CONTROL_8		(NS_USB_FIFO_STATUS_8)
#define	NS_USB_FIFO_STATUS_9		(0x3140) // FIFO Status and Control #9
#define	NS_USB_FIFO_CONTROL_9		(NS_USB_FIFO_STATUS_9)
#define	NS_USB_FIFO_STATUS_10		(0x3148) // FIFO Status and Control #10
#define	NS_USB_FIFO_CONTROL_10		(NS_USB_FIFO_STATUS_10)
#define	NS_USB_FIFO_STATUS_11		(0x3150) // FIFO Status and Control #11
#define	NS_USB_FIFO_CONTROL_11		(NS_USB_FIFO_STATUS_11)
#define	NS_USB_FIFO_STATUS_12		(0x3158) // FIFO Status and Control #12
#define	NS_USB_FIFO_CONTROL_12		(NS_USB_FIFO_STATUS_12)
#define	NS_USB_FIFO_STATUS_13		(0x3160) // FIFO Status and Control #13
#define	NS_USB_FIFO_CONTROL_13		(NS_USB_FIFO_STATUS_13)

/* PCI Arbiter Configuration registers */
#define	NS_PCI_PARBCFG			(0x0000) // PCI Arbiter Configuration
#define	NS_PCI_PARBINT			(0x0004) // PCI Arbiter Interrupt Status
#define	NS_PCI_PARBINTEN		(0x0008) // PCI Arbiter Interrupt Enable
#define	NS_PCI_PMISC			(0x000c) // PCI Miscellaneous Support
#define	NS_PCI_PCFG0			(0x0010) // PCI Configuration 0
#define	NS_PCI_PCFG1			(0x0014) // PCI Configuration 1
#define	NS_PCI_PCFG2			(0x0018) // PCI Configuration 2
#define	NS_PCI_PCFG3			(0x001c) // PCI Configuration 3
#define	NS_PCI_PAHBCFG			(0x0020) // PCI Bridge Configuration
#define	NS_PCI_PAHBERR			(0x0024) // PCI Bridge AHB Error Address
#define	NS_PCI_PCIERR			(0x0028) // PCI Bridge PCI Error Address
#define	NS_PCI_PINT			(0x002c) // PCI Bridge Interrupt Status
#define	NS_PCI_PINTEN			(0x0030) // PCI Bridge Interrupt Enable
#define	NS_PCI_PALTMEM0			(0x0034) // PCI Bridge AHB to PCI Memory Address Translate 0
#define	NS_PCI_PALTMEM1			(0x0038) // PCI Bridge AHB to PCI Memory Address Translate 1
#define	NS_PCI_PALTIO			(0x003c) // PCI Bridge AHB to PCI IO Address Translate
#define	NS_PCI_PMALT0			(0x0040) // PCI Bridge PCI to AHB Memory Address Translate 0
#define	NS_PCI_PMALT1			(0x0044) // PCI Bridge PCI to AHB Memory Address Translate 1
#define	NS_PCI_PALTCTL			(0x0048) // PCI Bridge Address Translation Control
#define	NS_PCI_CMISC			(0x004c) // CardBus Miscellaneous Support
#define	NS_PCI_CSKTEV			(0x1000) // CardBus Socket Event
#define	NS_PCI_CSKTMSK			(0x1004) // CardBus Socket Mask
#define	NS_PCI_CSKTPST			(0x1008) // CardBus Socket Present State
#define	NS_PCI_CSKTFEV			(0x100c) // CardBus Socket Force Event
#define	NS_PCI_CSKTCTL			(0x1010) // CardBus Socket Control

/*
 * FIXME -- we accidentally used undefined macro to setup sys module.
 * gcc 4.1 detected undefined but using the right value caused cpu to
 * freeze.  this will be fixed in hermit-at 1.1.9+1.
 */
#define NS_SCM_MEM_CS0_DYNAMIC_BASE	(0)
#define NS_SCM_MEM_CS1_DYNAMIC_BASE	(0)
#define NS_SCM_MEM_CS2_DYNAMIC_BASE	(0)
#define NS_SCM_MEM_CS3_DYNAMIC_BASE	(0)
#define NS_SCM_MEM_CS4_DYNAMIC_BASE	(0)
#define NS_SCM_MEM_CS5_DYNAMIC_BASE	(0)
#define NS_SCM_MEM_CS6_DYNAMIC_BASE	(0)
#define NS_SCM_MEM_CS7_DYNAMIC_BASE	(0)
#define NS_SCM_MEM_CS0_DYNAMIC_MASK	(0)
#define NS_SCM_MEM_CS1_DYNAMIC_MASK	(0)
#define NS_SCM_MEM_CS2_DYNAMIC_MASK	(0)
#define NS_SCM_MEM_CS3_DYNAMIC_MASK	(0)
#define NS_SCM_MEM_CS4_DYNAMIC_MASK	(0)
#define NS_SCM_MEM_CS5_DYNAMIC_MASK	(0)
#define NS_SCM_MEM_CS6_DYNAMIC_MASK	(0)
#define NS_SCM_MEM_CS7_DYNAMIC_MASK	(0)

#endif /* __ASM_HARDWARE_NS9750_H */
