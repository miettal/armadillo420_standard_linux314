#ifndef _MICROBLAZE_FLASH_SPI_H_
#define _MICROBLAZE_FLASH_SPI_H_

#define SPI_BASE_ADDR 0xff000000

/* opb spi registers */
#define REG_IPIF_SOFT_RESET       (SPI_BASE_ADDR + 0x40)
#define REG_SPICR                 (SPI_BASE_ADDR + 0x60)
#define     SPICR_MTI             (1<<8)
#define     SPICR_MSSSAE          (1<<7)
#define     SPICR_RX_FIFO_RESET   (1<<6)
#define     SPICR_TX_FIFO_RESET   (1<<5)
#define     SPICR_CPHA            (1<<4)
#define     SPICR_CPOL            (1<<3)
#define     SPICR_MASTER          (1<<2)
#define     SPICR_SPE             (1<<1)
#define     SPICR_LOOP            (1<<0)
#define REG_SPISR                 (SPI_BASE_ADDR + 0x64)
#define     SPISR_MODF            (1<<4)
#define     SPISR_TX_FULL         (1<<3)
#define     SPISR_TX_EMPTY        (1<<2)
#define     SPISR_RX_FULL         (1<<1)
#define     SPISR_RX_EMPTY        (1<<0)
#define REG_SPIDTR                (SPI_BASE_ADDR + 0x68)
#define REG_SPIDRR                (SPI_BASE_ADDR + 0x6C)
#define REG_SPISSR                (SPI_BASE_ADDR + 0x70)
#define REG_TX_FIFO_OCY           (SPI_BASE_ADDR + 0x74)
#define REG_RX_FIFO_OCY           (SPI_BASE_ADDR + 0x78)

#define REG_DGIE                  (SPI_BASE_ADDR + 0x1C)
#define     DGIE_GIE              (1<<31)
#define REG_IPISR                 (SPI_BASE_ADDR + 0x20)
#define REG_IPIER                 (SPI_BASE_ADDR + 0x28)

/* spi instructions */
#define WREN                      (0x06)
#define WRDI                      (0x04)
#define RDID                      (0x9f)
#define RDSR                      (0x05)
#define RDSR_WIP                  (1<<0)
#define RDSR_WEL                  (1<<1)
#define RDSR_BP0                  (1<<2)
#define RDSR_BP1                  (1<<3)
#define RDSR_BP2                  (1<<4)
#define RDSR_SRWD                 (1<<7)
#define WRSR                      (0x01)
#define READ                      (0x03)
#define FAST_READ                 (0x0b)
#define PP                        (0x02)
#define SE                        (0xd8)
#define BE                        (0xc7)
#define RES                       (0xab)

/* Dummy byte */
#define DUMMY                     (0x00)

#define __spi_write32(addr,val)	write32(addr,val)
#define __spi_read32(addr)	read32(addr)

#endif
