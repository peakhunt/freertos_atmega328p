#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "shell.h"
#include "pwm.h"
#include "spi_bus_master.h"
#include "spi_test.h"
#include "i2c_bus_master.h"
#include "baro_reader.h"
#include "adc.h"
#include "adc_reader.h"

void
vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
    StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize )
{
  static StaticTask_t       idleTCBBuffer;
  static StackType_t        idleStack[configIDLE_STACK_SIZE];

  *ppxIdleTaskTCBBuffer   = &idleTCBBuffer;
  *ppxIdleTaskStackBuffer = idleStack;
  *pulIdleTaskStackSize   = configIDLE_STACK_SIZE;
}

#define PERIOD1       400
#define PERIOD2       300
#define PERIOD3       200
#define PERIOD4       70

static void
init_led(void)
{
#if 0   // pin conflict with H/W SPI
  DDRD |= 0xf2;
  DDRB |= 0x20;
#endif
}

static void
LEDTask3(void* pvParameters)
{
	while(1)
	{
#if 0
    PORTB = (PORTB ^ _BV(PB5));
    vTaskDelay(PERIOD4 / portTICK_PERIOD_MS);

    PORTB = (PORTB ^ _BV(PB5));
    vTaskDelay(PERIOD3 / portTICK_PERIOD_MS);

    PORTB = (PORTB ^ _BV(PB5));
    vTaskDelay(PERIOD2 / portTICK_PERIOD_MS);

    PORTB = (PORTB ^ _BV(PB5));
#endif
    vTaskDelay(PERIOD1 / portTICK_PERIOD_MS);
	}
}

static void
init_tasks(void)
{
  static uint8_t        _tsk_stack[configMINIMAL_STACK_SIZE];
  static StaticTask_t   _tsk_buffer;

  xTaskCreateStatic(LEDTask3,
      "BTask3",
      configMINIMAL_STACK_SIZE,
      NULL,
      tskIDLE_PRIORITY + 1,
      _tsk_stack,
      &_tsk_buffer);
}

int main(void) __attribute__((OS_main));

int
main(void)
{
  init_led();
  pwm_init();

  adc_init(ADCRef_AVCC, ADCPrescaler_DIV_128);
  spi_bus_master_init();
  i2c_bus_master_init(0);

	shell_init();
  init_tasks();

  spi_test_init();
  baro_reader_init();
  adc_reader_init();

  vTaskStartScheduler();
  while(1)
  {
    // should never reach here
  }
  return 0;
}
