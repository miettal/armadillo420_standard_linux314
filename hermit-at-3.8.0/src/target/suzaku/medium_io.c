#include <io.h>
#include <arch/ioregs.h>

int change_console(char *console)
{
	/* nothing to do */
	return 0;
}

int arch_getchar(unsigned long timeout)
{
        while (!(read32(UART_STATUS) & UART_STATREG_RX_FIFO_VALID_DATA));
        return read32(UART_RECEIVE_FIFO) & 0xff;
}

int arch_putchar(int c, unsigned long timeout)
{
        while (read32(UART_STATUS) & UART_STATREG_TX_FIFO_FULL);
        write32(UART_TRANSMIT_FIFO, c & 0xff);
        return 0;
}
