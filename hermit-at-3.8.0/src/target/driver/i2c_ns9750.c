#include <io.h>
#include <arch/ioregs.h>
#include <delay.h>

#define CMD_NOP    0x000
#define CMD_READ   0x400
#define CMD_WRITE  0x500
#define CMD_STOP   0x600
#define CMD_TXVAL  0x2000

#define IRQ_M_TX_DATA 0x300
#define IRQ_M_RX_DATA 0x400
#define IRQ_M_CMD_ACK 0x500
#define IRQ_MASK      0xf00

static int i2c_send_cmd(unsigned long cmd){
	write32(NS9750_I2C_PHYS_BASE + NS_I2C_CMD_TX_DATA, cmd);
	return 0;
}

static int i2c_wait_irq(unsigned long irq){
	unsigned long status;
	while(1){
		status = read32(NS9750_I2C_PHYS_BASE + NS_I2C_STATUS_RX_DATA);
		if((status & IRQ_MASK) == irq){
			if(irq == IRQ_M_RX_DATA){
				return (int)(status & 0xff);
			}else{
				return 0;
			}
		}
	}
}

static int i2c_read(unsigned long addr)
{
	int value;

	mdelay(10);

	i2c_send_cmd(CMD_WRITE | CMD_TXVAL | addr);
	i2c_wait_irq(IRQ_M_TX_DATA);

	i2c_send_cmd(CMD_READ);
	value = i2c_wait_irq(IRQ_M_RX_DATA);

	i2c_send_cmd(CMD_STOP);
	i2c_wait_irq(IRQ_M_CMD_ACK);

	return value;
}

static int i2c_write(unsigned long addr, unsigned char c)
{
	mdelay(10);

	i2c_send_cmd(CMD_WRITE | CMD_TXVAL | addr);
	i2c_wait_irq(IRQ_M_TX_DATA);

	i2c_send_cmd(CMD_NOP | CMD_TXVAL | c);
	i2c_wait_irq(IRQ_M_TX_DATA);

	i2c_send_cmd(CMD_STOP);
	i2c_wait_irq(IRQ_M_CMD_ACK);

	return 0;
}

static void i2c_init(void){
	write32(NS9750_I2C_PHYS_BASE + NS_I2C_CONFIG, 0x1f4 | 0x0200); //50Khz
	write32(NS9750_I2C_PHYS_BASE + NS_I2C_MASTER_ADDR, 0xa0);
	write32(NS9750_I2C_PHYS_BASE + NS_I2C_SLAVE_ADDR, 0x00);
}

static int eeprom_getc(unsigned long addr){
	return i2c_read(addr);
}

static int eeprom_putc(unsigned long addr, unsigned int ch){
	return i2c_write(addr, ch);
}

int ns9750_get_mac(unsigned char *mac){
	int i;
	int ch;

	i2c_init();

	for(i=0; i<6; i++){
		ch = eeprom_getc(i);
		if(ch == -1){
			return -1;
		}
		mac[i] = ch;
	}
	return 0;
}

int ns9750_set_mac(unsigned char *mac){
	int i;

	i2c_init();

	for(i=0; i<6; i++){
		eeprom_putc(i, mac[i]);
	}
	return 0;
}
