#ifndef _IOREGS_H_
#define _IOREGS_H_

#include "xparameters.h"

#define UART_BASE_ADDR                  (XPAR_CONSOLE_UART_BASEADDR)
#define INTC_BASE_ADDR                  (XPAR_SYSTEM_INTC_BASEADDR)
#define SYSREG_BASE_ADDR                (XPAR_OPB_GPIO_0_BASEADDR)

#define UART_RECEIVE_FIFO		(UART_BASE_ADDR + 0x00)
#define UART_TRANSMIT_FIFO		(UART_BASE_ADDR + 0x04)
#define UART_STATUS			(UART_BASE_ADDR + 0x08)
#define UART_CONTROL			(UART_BASE_ADDR + 0x12)

#define SYSREG				(SYSREG_BASE_ADDR + 0x00)

#define UART_STATREG_RX_FIFO_VALID_DATA (1<<0)
#define UART_STATREG_RX_FIFO_FULL       (1<<1)
#define UART_STATREG_TX_FIFO_EMPTY      (1<<2)
#define UART_STATREG_TX_FIFO_FULL       (1<<3)
#define UART_STATREG_INTR_ENABLED       (1<<4)
#define UART_STATREG_OVERUN_ERROR       (1<<5)
#define UART_STATREG_FRAME_ERROR        (1<<6)
#define UART_STATREG_PAR_ERROR          (1<<7)

#define UART_CTRL_REG_RST_TX_FIFO       (1<<0)
#define UART_CTRL_REG_RST_RX_FIFO       (1<<1)
#define UART_CTRL_REG_ENABLE_INTR       (1<<4)

#define SYSREG_REBOOT                   (1<<0)
#define SYSREG_COMPAT                   (1<<1)
#define SYSREG_AUTOBOOT                 (1<<2)
#define SYSREG_FLASH_SPI                (1<<3)

#endif	/* _IOREGS_H_ */
