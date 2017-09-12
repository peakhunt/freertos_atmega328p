#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "spi_test.h"
#include "spi_bus_master.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bmp180.h"

#include "baro_reader.h"

extern int8_t BMP180_I2C_routine(struct bmp180_t* bmp180);

static TaskHandle_t     _baro_task_handle;
static struct bmp180_t  _bmp180;

volatile int32_t     _baro_press;
volatile int16_t     _baro_temp;
volatile uint32_t    _baro_loop = 0;

static void
baro_task(void* pvParameters)
{
  uint32_t    uncomp_press;
  uint16_t    uncomp_temp;

  BMP180_I2C_routine(&_bmp180);
  bmp180_init(&_bmp180);

  while(1)
  {
    uncomp_press = bmp180_get_uncomp_pressure();
    uncomp_temp  = bmp180_get_uncomp_temperature();

    _baro_press = bmp180_get_pressure(uncomp_press);
    _baro_temp  = bmp180_get_temperature(uncomp_temp);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    _baro_loop++;
  }
}


void
baro_reader_init(void)
{
  #define S_SIZE    128
  static uint8_t        _tsk_stack[S_SIZE];
  static StaticTask_t   _tsk_buffer;

  _baro_task_handle = 
  xTaskCreateStatic(baro_task,
      "Baro",
      S_SIZE,
      NULL,
      tskIDLE_PRIORITY + 1,
      _tsk_stack,
      &_tsk_buffer);
}
