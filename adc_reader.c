#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "adc.h"
#include "adc_reader.h"

//
// use ADC0-3 as analog input
// PC0-PC3
//
static TaskHandle_t     _adc_task_handle;
static QueueHandle_t    _adc_q;
volatile uint16_t       _adc_values[4];

static void
adc_conv_complete(uint8_t chnl, uint16_t sample)
{
  xQueueSendFromISR(_adc_q, (void*)&sample, NULL);
}

static void
adc_task(void* pvParameters)
{
  uint8_t   current = 0;
  uint16_t  sample;

  while(1)
  {
    adc_start_conversion(current, adc_conv_complete);

    while(xQueueReceive(_adc_q, &sample, portMAX_DELAY) != pdPASS)
    {
    }

    _adc_values[current] = sample;

    vTaskDelay(10 / portTICK_PERIOD_MS);
    current = (current + 1) % 4;
  }
}

void
adc_reader_init(void)
{
  static uint8_t        _tsk_stack[configMINIMAL_STACK_SIZE];
  static StaticTask_t   _tsk_buffer;

  static uint16_t       _adc_q_buf[4];
  static StaticQueue_t  _adc_q_struct;

  _adc_q = xQueueCreateStatic(4, sizeof(uint16_t), (void*)_adc_q_buf, &_adc_q_struct);

  _adc_task_handle = 
  xTaskCreateStatic(adc_task,
      "ADC",
      configMINIMAL_STACK_SIZE,
      NULL,
      tskIDLE_PRIORITY + 1,
      _tsk_stack,
      &_tsk_buffer);
}
