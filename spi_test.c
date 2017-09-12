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

#include "tm1638.h"

static TaskHandle_t     _spi_task_handle;
uint8_t                 _spi_test_cnt;

#if 0
static void
spi_done(void)
{
  xTaskNotifyFromISR(_spi_task_handle, 
      0x01,
      eSetBits,
      NULL);
}
#endif

static void
spi_task(void* pvParameters)
{
  int i;
  uint16_t    cnt = 0,
              on_off = 1;

  TM1638_init_Display();

  for(i = 0; i < 8; i++)
  {
    TM1638_ShowDig(i, i, 1);
  }
  TM1638_SetupDisplay(1, 7);

  TM1638_ShowLeds(TM1638_LED_GRE);

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  TM1638_ClearDig(0, 0);
  TM1638_ClearDig(1, 0);
  TM1638_ClearDig(2, 0);
  TM1638_ClearDig(3, 0);

  TM1638_ShowString("ABCD", 0, 0);

  i = 1;
  while(1)
  {
#if 1
    TM1638_ShowDecNumber(cnt, 0, 4);
    TM1638_ShowLed(i, on_off ? TM1638_LED_RED : TM1638_LED_NON);

    i++;
    if(i >= 9)
    {
      i = 1;
      on_off = !on_off;
    }
    cnt++;
    if(cnt > 9999)
    {
      cnt = 0;
    }
#endif
#if 0
    spi_bus_master_read_write_sync(0, tx_buf, rx_buf, 8, spi_bus_chip_select);
    uint32_t  v;

    spi_bus_master_read_write_async(0, tx_buf, rx_buf, 8,
        spi_bus_chip_select,
        spi_done);

    xTaskNotifyWait(0x00, 0xffffffff, &v, portMAX_DELAY);

    //
    // if first bit of v is set, that means spi transaction is complete.
    // 
#endif
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void
spi_test_init(void)
{
  static uint8_t        _tsk_stack[128];
  static StaticTask_t   _tsk_buffer;

  TM1638_Ports_init();

  _spi_task_handle = 
  xTaskCreateStatic(spi_task,
      "SPI",
      128,
      NULL,
      tskIDLE_PRIORITY + 1,
      _tsk_stack,
      &_tsk_buffer);
}
