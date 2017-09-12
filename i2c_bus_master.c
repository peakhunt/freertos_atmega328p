#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "i2c_bus_master.h"

////////////////////////////////////////////////////////////////////////////////
//
// private definitions
//
////////////////////////////////////////////////////////////////////////////////
#define TWI_START                  0x08
#define TWI_REPEAT_START           0x10
#define TWI_ARBITRATION_LOST       0x38
#define TWI_BUS_ERROR              0x00
#define TWI_MASTER_TX_ADDR_ACK     0x18
#define TWI_MASTER_TX_ADDR_NACK    0x20
#define TWI_MASTER_TX_DATA_ACK     0x28
#define TWI_MASTER_TX_DATA_NACK    0x30
#define TWI_MASTER_RX_ADDR_ACK     0x40
#define TWI_MASTER_RX_ADDR_NACK    0x48
#define TWI_MASTER_RX_DATA_ACK     0x50
#define TWI_MASTER_RX_DATA_NACK    0x58

typedef uint8_t i2cflags_t;

typedef struct
{
  volatile i2cflags_t               errors;
  volatile i2caddr_t                addr;
  volatile const uint8_t            *txbuf;
  volatile size_t                   txbytes;
  volatile size_t                   txidx;
  volatile uint8_t                  *rxbuf;
  volatile size_t                   rxbytes;
  volatile size_t                   rxidx;
  volatile uint8_t                  done;
} I2CDriverState;

////////////////////////////////////////////////////////////////////////////////
//
// module privates
//
////////////////////////////////////////////////////////////////////////////////
static I2CDriverState     _i2c_state;

////////////////////////////////////////////////////////////////////////////////
//
// private utilities
//
////////////////////////////////////////////////////////////////////////////////
static void
_i2c_wakeup_isr(I2CDriverState* i2cp)
{
  i2cp->done  = 1;
}

static void
_i2c_wakeup_error_isr(I2CDriverState* i2cp)
{
  i2cp->done  = 1;
}


////////////////////////////////////////////////////////////////////////////////
//
// I2C ISR
//
////////////////////////////////////////////////////////////////////////////////
ISR(TWI_vect)
{
  I2CDriverState* i2cp = &_i2c_state;

  switch (TWSR & 0xF8)
  {
  case TWI_START:
  case TWI_REPEAT_START:
    TWDR = (i2cp->addr << 1);
    if ((i2cp->txbuf == NULL) || (i2cp->txbytes == 0) || (i2cp->txidx == i2cp->txbytes))
    {
      TWDR |= 0x01;
    }
    TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE));
    break;

  case TWI_MASTER_TX_ADDR_ACK:
  case TWI_MASTER_TX_DATA_ACK:
    if (i2cp->txidx < i2cp->txbytes)
    {
      TWDR = i2cp->txbuf[i2cp->txidx++];
      TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE));
    }
    else
    {
      if (i2cp->rxbuf && i2cp->rxbytes)
      {
        TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE));
      }
      else
      {
        TWCR = ((1 << TWSTO) | (1 << TWINT) | (1 << TWEN));
        _i2c_wakeup_isr(i2cp);
      }
    }
    break;

  case TWI_MASTER_RX_ADDR_ACK:
    if (i2cp->rxidx == (i2cp->rxbytes - 1))
    {
      TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE));
    }
    else
    {
      TWCR = ((1 << TWEA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE));
    }
    break;

  case TWI_MASTER_RX_DATA_ACK:
    i2cp->rxbuf[i2cp->rxidx++] = TWDR;
    if (i2cp->rxidx == (i2cp->rxbytes - 1))
    {
      TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE));
    }
    else
    {
      TWCR = ((1 << TWEA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE));
    }
    break;

  case TWI_MASTER_RX_DATA_NACK:
    i2cp->rxbuf[i2cp->rxidx] = TWDR;
    TWCR = ((1 << TWSTO) | (1 << TWINT) | (1 << TWEN));
    _i2c_wakeup_isr(i2cp);

  case TWI_MASTER_TX_ADDR_NACK:
  case TWI_MASTER_TX_DATA_NACK:
  case TWI_MASTER_RX_ADDR_NACK:
    i2cp->errors |= I2C_ACK_FAILURE;
    break;

  case TWI_ARBITRATION_LOST:
    i2cp->errors |= I2C_ARBITRATION_LOST;
    break;

  case TWI_BUS_ERROR:
    i2cp->errors |= I2C_BUS_ERROR;
    break;
  
  default:
    /* FIXME: only gets here if there are other MASTERs in the bus */
    TWCR = ((1 << TWSTO) | (1 << TWINT) | (1 << TWEN));
    _i2c_wakeup_error_isr(i2cp);
  }

  if (i2cp->errors != I2C_NO_ERROR)
  {
    TWCR = ((1 << TWSTO) | (1 << TWINT) | (1 << TWEN));
    _i2c_wakeup_error_isr(i2cp);
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
i2c_bus_master_init(uint32_t clock_speed)
{
  if(clock_speed == 0)
  {
    clock_speed = 100000;
  }

  /* TODO: Test TWI without external pull-ups (use internal) */

  /* Configure prescaler to 1 */
  TWSR &= 0xF8;

  /* Configure baudrate */
  TWBR = ((F_CPU / clock_speed) - 16) / 2;
}

uint8_t
i2c_bus_master_tx_sync(i2caddr_t  addr, uint8_t* tx, uint16_t txbytes,
    uint8_t* rx, uint16_t rxbytes)
{
  I2CDriverState* i2cp = &_i2c_state;

  i2cp->errors    = I2C_NO_ERROR;
  i2cp->addr      = addr;
  i2cp->txbuf     = tx;
  i2cp->txbytes   = txbytes;
  i2cp->txidx     = 0;
  i2cp->rxbuf     = rx;
  i2cp->rxbytes   = rxbytes;
  i2cp->rxidx     = 0;
  i2cp->done      = 0;

  TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE));

  while(!i2cp->done)
  {
    // busy loop
  }

  if(i2cp->errors == I2C_NO_ERROR)
  {
    return 1;
  }
  return 0;
}

uint8_t
i2c_bus_master_rx_sync(i2caddr_t addr, uint8_t* rx, uint16_t rxbytes)
{
  I2CDriverState* i2cp = &_i2c_state;

  i2cp->errors    = I2C_NO_ERROR;
  i2cp->addr      = addr;
  i2cp->txbuf     = NULL;
  i2cp->txbytes   = 0;
  i2cp->txidx     = 0;
  i2cp->rxbuf     = rx;
  i2cp->rxbytes   = rxbytes;
  i2cp->rxidx     = 0;
  i2cp->done      = 0;

  /* Send START */
  TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE));

  while(!i2cp->done)
  {
    // busy loop
  }

  if(i2cp->errors == I2C_NO_ERROR)
  {
    return 1;
  }
  return 0;
}
