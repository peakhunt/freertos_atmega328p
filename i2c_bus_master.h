#ifndef __I2C_BUS_MASTER_DEF_H__
#define __I2C_BUS_MASTER_DEF_H__

#define I2C_NO_ERROR               0x00
#define I2C_BUS_ERROR              0x01
#define I2C_ARBITRATION_LOST       0x02
#define I2C_ACK_FAILURE            0x04
#define I2C_OVERRUN                0x08
#define I2C_PEC_ERROR              0x10
#define I2C_TIMEOUT                0x20
#define I2C_SMB_ALERT              0x40

typedef uint8_t i2caddr_t;

extern void i2c_bus_master_init(uint32_t clock_speed);
extern uint8_t i2c_bus_master_tx_sync(i2caddr_t  addr, uint8_t* tx, uint16_t txbytes,
																								uint8_t* rx, uint16_t rxbytes);
extern uint8_t i2c_bus_master_rx_sync(i2caddr_t addr, uint8_t* rx, uint16_t rxbytes);

#endif //!__I2C_BUS_MASTER_DEF_H__
