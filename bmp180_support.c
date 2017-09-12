#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "bmp180.h"

#include "i2c_bus_master.h"

#include "FreeRTOS.h"
#include "task.h"

#define C_BMP180_ONE_U8X        1

#ifdef BMP180_API

s8 BMP180_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMP180_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMP180_I2C_routine(struct bmp180_t* bmp180);
#endif

void BMP180_delay_msek(u32 msek);

#ifdef BMP180_API
s8 BMP180_I2C_routine(struct bmp180_t* bmp180)
{
	bmp180->bus_write   = BMP180_I2C_bus_write;
	bmp180->bus_read    = BMP180_I2C_bus_read;
	bmp180->dev_addr    = BMP180_I2C_ADDR;
	bmp180->delay_msec  = BMP180_delay_msek;

	return BMP180_INIT_VALUE;
}

#define	I2C_BUFFER_LEN 32
#define I2C0 5

s8 BMP180_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMP180_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN];
	u8 stringpos = BMP180_INIT_VALUE;

	array[BMP180_INIT_VALUE] = reg_addr;

	for (stringpos = BMP180_INIT_VALUE; stringpos < cnt; stringpos++)
  {
		array[stringpos + C_BMP180_ONE_U8X] = *(reg_data + stringpos);
	}

  // hkim implementation
  i2c_bus_master_tx_sync(dev_addr, array, cnt + C_BMP180_ONE_U8X, NULL, 0);

	return (s8)iError;
}

s8 BMP180_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMP180_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN] = {BMP180_INIT_VALUE};
	u8 stringpos = BMP180_INIT_VALUE;
	array[BMP180_INIT_VALUE] = reg_addr;

  // hkim implementation
  i2c_bus_master_tx_sync(dev_addr, array, C_BMP180_ONE_U8X, array, cnt);
  //i2c_bus_master_rx_sync(dev_addr, array, cnt);

	for (stringpos = BMP180_INIT_VALUE; stringpos < cnt; stringpos++) {
		*(reg_data + stringpos) = array[stringpos];
	}
	return (s8)iError;
}

void BMP180_delay_msek(u32 msek)
{
  vTaskDelay(msek / portTICK_PERIOD_MS);
}

#endif
