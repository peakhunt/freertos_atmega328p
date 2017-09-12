#ifndef __SPI_BUS_MASTER_DEF_H__
#define __SPI_BUS_MASTER_DEF_H__

typedef enum
{
  SPIBus_Data_Order_MSB,
  SPIBus_Data_Order_LSB,
} SPIBus_Data_Order;

typedef enum
{
  SPIBus_CPOL_High_Idle,
  SPIBus_CPOL_Low_Idle,
} SPIBus_CPOL;

typedef enum
{
  SPIBus_Speed_DIV_2,
  SPIBus_Speed_DIV_4,
  SPIBus_Speed_DIV_8,
  SPIBus_Speed_DIV_16,
  SPIBus_Speed_DIV_32,
  SPIBus_Speed_DIV_64,
  SPIBus_Speed_DIV_128,
} SPIBus_Speed;

typedef enum
{
  SPIBus_CPHA_Leading,
  SPIBus_CPHA_Trailing,
} SPIBus_CPHA;

typedef void (*spi_cs_callback)(uint8_t ns, uint8_t begin_end);
typedef void (*spi_complete_callback)(void);

extern void spi_bus_master_init(void);
extern void spi_bus_master_register_slave(uint8_t ns,
    SPIBus_Data_Order order,
    SPIBus_CPOL       cpol,
    SPIBus_CPHA       cpha,
    SPIBus_Speed      speed);
extern void spi_bus_master_read_write_sync(uint8_t ncs, uint8_t* write, uint8_t* read, uint16_t len,
    spi_cs_callback cscb);

extern void spi_bus_master_read_write_async(uint8_t ncs, uint8_t* write, uint8_t* read, uint16_t len,
    spi_cs_callback cscb,
    spi_complete_callback ccb);

#endif //!__SPI_BUS_MASTER_DEF_H__
