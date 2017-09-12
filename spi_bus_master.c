#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "spi_bus_master.h"

////////////////////////////////////////////////////////////////////////////////
//
// private definitions
//
////////////////////////////////////////////////////////////////////////////////
#define       SPI_MASTER_MAX_SLAVES         4

typedef struct
{
  SPIBus_Data_Order   data_order;
  SPIBus_CPOL         cpol;  
  SPIBus_CPHA         cpha;
  SPIBus_Speed        speed;
} SPIBusSlave;

typedef struct
{
  volatile uint8_t                  ncs;
  volatile uint8_t*                 rx_buf;
  volatile uint8_t*                 tx_buf;
  volatile uint16_t                 len;
  volatile uint16_t                 ndx;
  volatile spi_complete_callback    ccb;
  volatile spi_cs_callback          cscb;
} SPICompleteCallbackCB;

////////////////////////////////////////////////////////////////////////////////
//
// module privates
//
////////////////////////////////////////////////////////////////////////////////
static SPIBusSlave    _slaves[SPI_MASTER_MAX_SLAVES];
static SPICompleteCallbackCB    _async_ctx;

////////////////////////////////////////////////////////////////////////////////
//
// SPI controller configuration
//
////////////////////////////////////////////////////////////////////////////////
static void
spi_bus_config_master(void)
{
  // PB5 : SCK
  // PB4 : MISO
  // PB3 : MOSI
  // PB2 : SS. we don't use it and the pin must be used as OUTPUT or 
  //       SPI Controller will be seriously fucked up.
  //
  // XXX: what about three wire SPI ???
  //
  DDRB |=  (_BV(PB5) | _BV(PB3));
  DDRB &= ~(_BV(PB4));

  // set to master mode
  //SPCR = _BV(MSTR) | _BV(SPE);
  SPCR = _BV(MSTR);
}

static inline void
spi_bus_config_speed(SPIBus_Speed speed)
{
  // clear all the bits first
  SPCR  &= ~(_BV(SPR1) | _BV(SPR0));
  SPSR  &= ~(_BV(SPI2X));

  switch(speed)
  {
  case SPIBus_Speed_DIV_2:
    SPSR  |= _BV(SPI2X);
    break;

  case SPIBus_Speed_DIV_4:
    break;

  case SPIBus_Speed_DIV_8:
    SPSR  |= _BV(SPI2X);
    SPCR  |= _BV(SPR0);
    break;

  case SPIBus_Speed_DIV_16:
    SPCR  |= _BV(SPR0);
    break;

  case SPIBus_Speed_DIV_32:
    SPSR  |= _BV(SPI2X);
    SPCR  |= _BV(SPR1);
    break;

  case SPIBus_Speed_DIV_64:
    SPSR  |= _BV(SPI2X);
    SPCR  |= (_BV(SPR0) | _BV(SPR1));
    break;

  case SPIBus_Speed_DIV_128:
    SPCR  |= (_BV(SPR0) | _BV(SPR1));
    break;
  }
}

static void
spi_bus_config_data_order(SPIBus_Data_Order order)
{
  switch(order)
  {
  case SPIBus_Data_Order_MSB:
    SPCR &= ~_BV(DORD);
    break;

  case SPIBus_Data_Order_LSB:
    SPCR |= _BV(DORD);
    break;
  }
}

static void
spi_bus_config_cpol_cpha(SPIBus_CPOL cpol, SPIBus_CPHA cpha)
{
  // clear first
  SPCR &= ~(_BV(CPOL) | _BV(CPHA));

  // cpol
  // 0 : SCK high when idle
  // 1 : SCK low  when idle
  switch(cpol)
  {
  case SPIBus_CPOL_High_Idle:
    SPCR |= _BV(CPOL);
    break;

  case SPIBus_CPOL_Low_Idle:
    break;
  }
  
  // cpha
  // 0 : sampled at leading clock
  // 1 : sampled at trailing clock
  switch(cpha)
  {
  case SPIBus_CPHA_Leading:
    break;

  case SPIBus_CPHA_Trailing:
    SPCR |= _BV(CPHA);
    break;
  }
}

static void
spi_bus_config_master_for_slave(SPIBusSlave* slave)
{
  spi_bus_config_speed(slave->speed);
  spi_bus_config_data_order(slave->data_order);
  spi_bus_config_cpol_cpha(slave->cpol, slave->cpha);
}


////////////////////////////////////////////////////////////////////////////////
//
// SPI ISR
//
////////////////////////////////////////////////////////////////////////////////
ISR(SPI_STC_vect)
{
  // read first
  if(_async_ctx.rx_buf != NULL)
  {
    _async_ctx.rx_buf[_async_ctx.ndx] = SPDR;
  }

  _async_ctx.ndx++;

  if(_async_ctx.ndx >= _async_ctx.len)
  {
    _async_ctx.cscb(_async_ctx.ncs, 0);
    _async_ctx.ccb();

    // we are done
    SPCR &= ~(_BV(SPIE) | _BV(SPE));

    return;
  }

  SPDR = _async_ctx.tx_buf[_async_ctx.ndx];
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
spi_bus_master_init(void)
{
  uint8_t   i;

  spi_bus_config_master();

  for(i = 0; i < SPI_MASTER_MAX_SLAVES; i++)
  {
    _slaves[i].data_order   = SPIBus_Data_Order_MSB;
    _slaves[i].cpol         = SPIBus_CPOL_High_Idle;
    _slaves[i].cpha         = SPIBus_CPHA_Leading;
    _slaves[i].speed        = SPIBus_Speed_DIV_2;;
  }
}

void
spi_bus_master_read_write_sync(uint8_t ncs, uint8_t* write, uint8_t* read, uint16_t len, spi_cs_callback cscb)
{
  uint16_t   i;

  spi_bus_config_master_for_slave(&_slaves[ncs]);

  SPCR |= _BV(SPE);
  cscb(ncs, 1);

  for(i = 0; i < len; i++)
  {
    SPDR = write[i];

    while((SPSR & _BV(SPIF)) == 0)
    {
      // busy loop
    }

    if(read != NULL)
    {
      read[i] = SPDR;
    }
  }

  cscb(ncs, 0);
  SPCR &= ~_BV(SPE);
}

void
spi_bus_master_read_write_async(uint8_t ncs, uint8_t* write, uint8_t* read, uint16_t len,
    spi_cs_callback cscb,
    spi_complete_callback ccb)
{
  _async_ctx.ncs      = ncs;
  _async_ctx.rx_buf   = (volatile uint8_t*)read;
  _async_ctx.tx_buf   = (volatile uint8_t*)write;
  _async_ctx.len      = len;
  _async_ctx.ndx      = 0;
  _async_ctx.ccb      = (volatile spi_complete_callback)ccb;
  _async_ctx.cscb     = (volatile spi_cs_callback)cscb;

  spi_bus_config_master_for_slave(&_slaves[ncs]);

  SPCR |= (_BV(SPE) | _BV(SPIE));
  cscb(ncs, 1);

  SPDR = write[0];
}

void
spi_bus_master_register_slave(uint8_t ns,
    SPIBus_Data_Order order,
    SPIBus_CPOL       cpol,
    SPIBus_CPHA       cpha,
    SPIBus_Speed      speed)
{
  _slaves[ns].data_order    = order;
  _slaves[ns].cpol          = cpol;
  _slaves[ns].cpha          = cpha;
  _slaves[ns].speed         = speed;;
}
