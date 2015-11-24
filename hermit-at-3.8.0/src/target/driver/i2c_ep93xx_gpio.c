#include <arch/ioregs.h>
#include <arch/regs_eth.h>
#include <io.h>
#include <delay.h>
#include <i2c_ep93xx_gpio.h>

#define I2C_SDA_BIT BIT(4)
#define I2C_SCL_BIT BIT(5)

#define i2c_delay() udelay(10)

static void i2c_set_sda(void){
	write32(GPIO_PBDDR, read32(GPIO_PBDDR) | I2C_SDA_BIT);
	write32(GPIO_PBDR, read32(GPIO_PBDR) | I2C_SDA_BIT);
	i2c_delay();
}

static void i2c_clr_sda(void){
	write32(GPIO_PBDDR, read32(GPIO_PBDDR) | I2C_SDA_BIT);
	write32(GPIO_PBDR, read32(GPIO_PBDR) & ~I2C_SDA_BIT);
	i2c_delay();
}

static int i2c_read_sda(void){
	unsigned long val;
	write32(GPIO_PBDDR, read32(GPIO_PBDDR) & ~I2C_SDA_BIT);
	val = (read32(GPIO_PBDR) & I2C_SDA_BIT);
	i2c_delay();
	return (val ? 1 : 0);
}

static void i2c_set_scl(void){
	write32(GPIO_PBDDR, read32(GPIO_PBDDR) | I2C_SCL_BIT);
	write32(GPIO_PBDR, read32(GPIO_PBDR) | I2C_SCL_BIT);
	i2c_delay();
}

static void i2c_clr_scl(void){
	write32(GPIO_PBDDR, read32(GPIO_PBDDR) | I2C_SCL_BIT);
	write32(GPIO_PBDR, read32(GPIO_PBDR) & ~I2C_SCL_BIT);
	i2c_delay();
}

static int i2c_ack(void){
	int timeout = 1000;
	write32(GPIO_PBDDR, read32(GPIO_PBDDR) & ~I2C_SDA_BIT);
	i2c_set_scl();
	while(timeout--){
		if(i2c_read_sda() == 0) break;
	}
	if(timeout < 0){
		hprintf("timeout acknowledge\n");
		return -1;
	}
	i2c_clr_scl();
	return 0;
}

static int i2c_read(void){
	unsigned char ch;
	unsigned char bit;

	int i;
	for(i=0, ch=0; i<8;i++){
		i2c_set_scl();
		bit = i2c_read_sda();
		i2c_clr_scl();

		ch |= (bit << (7-i));
	}
	return (int)ch;
}

static void i2c_write(unsigned char ch){
	unsigned char bit;
	int i;

	i2c_clr_scl();
	for(i=0; i<8; i++){
		bit = ((ch >> (7-i)) & 0x01);
		if(bit){
			i2c_set_sda();
		}else{
			i2c_clr_sda();
		}
		i2c_set_scl();
		i2c_clr_scl();
	}
}

static void i2c_start(void){
	i2c_set_sda();
	i2c_set_scl();
	i2c_clr_sda();
	i2c_clr_scl();
}

static void i2c_stop(void){
	i2c_clr_scl();
	i2c_clr_sda();
	i2c_set_scl();
	i2c_set_sda();
}

static void i2c_init(void){
	i2c_stop();
}

static int eeprom_getc(unsigned long addr){
	int ch;
	i2c_start();

	//Control
	i2c_write(0xa0);
	if(i2c_ack() == -1){
		i2c_stop();
		return -1;
	}

	//Address
	i2c_write((unsigned char)addr);
	if(i2c_ack() == -1){
		i2c_stop();
		return -1;
	}

	i2c_start();

	//Control
	i2c_write(0xa1);
	if(i2c_ack() == -1){
		i2c_stop();
		return -1;
	}

	//Data read
	ch = i2c_read();

	i2c_stop();

	return ch;
}

static int eeprom_putc(unsigned long addr, unsigned int ch){
	i2c_start();

	//Control
	i2c_write(0xa0);
	if(i2c_ack() == -1){
		i2c_stop();
		return -1;
	}

	//Address
	i2c_write((unsigned char)addr);
	if(i2c_ack() == -1){
		i2c_stop();
		return -1;
	}

	//Value
	i2c_write((unsigned char)ch);
	if(i2c_ack() == -1){
		i2c_stop();
		return -1;
	}

	i2c_stop();
	mdelay(10);

	return 0;
}

int ep93xx_get_mac(unsigned char *mac){
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

int ep93xx_set_mac(unsigned char *mac){
	int i;

	i2c_init();

	for(i=0; i<6; i++){
		eeprom_putc(i, mac[i]);
	}
	return 0;
}
