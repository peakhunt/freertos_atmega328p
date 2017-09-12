#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "shell.h"
#include "pwm.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// private definitions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SHELL_MAX_COLUMNS_PER_LINE      64
#define SHELL_DEFAULT_BAUD_RATE         115200
#define SHELL_COMMAND_MAX_ARGS          4
#define SHELL_MAX_COMMAND_LEN           32

#define UBRR(b)                         (((F_CPU / b) >> 4) - 1)
#define UBRR2x(b)                       (((F_CPU / b) >> 3) - 1)

#define VERSION       "328P Shell V0.2a"
#define SHELL_PROMPT  "\r\n328P> "

typedef void (*shell_command_handler)(int argc, const char** argv);

typedef struct
{
  const char*           command;
  const char*           description;
  shell_command_handler handler;
} ShellCommand;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// command prototypes
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void shell_command_help(int argc, const char** argv);
static void shell_command_version(int argc, const char** argv);
static void shell_command_pwm(int argc, const char** argv);
static void shell_command_spi_test(int argc, const char** argv);
static void shell_command_baro(int argc, const char** argv);
static void shell_command_adc(int argc, const char** argv);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// module private globals
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static char             _print_buffer[SHELL_MAX_COLUMNS_PER_LINE + 1];
static char             _cmd_buffer[SHELL_MAX_COMMAND_LEN + 1];
static uint8_t          _cmd_buffer_ndx = 0;


static QueueHandle_t        _rx_q,
                            _tx_q;

static ShellCommand     _commands[] =
{
  {
    "help",
    "show this command",
    shell_command_help,
  },
  {
    "version",
    "show version",
    shell_command_version,
  },
  {
    "pwm",
    "control pwm output",
    shell_command_pwm,
  },
  {
    "spi_test",
    "show spi test stat",
    shell_command_spi_test,
  },
  {
    "baro",
    "show baro data",
    shell_command_baro,
  },
  {
    "adc",
    "show adc",
    shell_command_adc,
  },
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IRQ handlers
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(USART_RX_vect)
{
  uint8_t       b;

  b = UDR0;

  xQueueSendFromISR(_rx_q, (void*)&b, NULL);
}

ISR(USART_UDRE_vect)
{
  uint8_t     b;

  if(xQueueReceiveFromISR(_tx_q, (void*)&b, NULL) == pdPASS)
  {
    UDR0 = b;
  }
  else
  {
    UCSR0B &= ~_BV(UDRIE0);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// private utilities
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//static inline void __attribute__((optimize("O0")))
static void
shell_send_data(uint8_t d)
{
  while(xQueueSend(_tx_q, &d, portMAX_DELAY) != pdPASS)
  {
  }
  UCSR0B |= _BV(UDRIE0);
}


static inline void
shell_print(char* str)
{
  int i = 0;

  for(i = 0; i < strlen(str); i++)
  {
    shell_send_data((uint8_t)str[i]);
  }
}

static void
shell_printf(const char* fmt, ...)
{
  static va_list   args;
  int       len,
            i;

  va_start(args, fmt);
  len = vsnprintf(_print_buffer, SHELL_MAX_COLUMNS_PER_LINE, fmt, args);
  va_end(args);

  for(i = 0; i < len; i++)
  {
    shell_send_data(_print_buffer[i]);
  }
}

static inline void
shell_prompt(void)
{
  shell_print(SHELL_PROMPT);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// usart initialization
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void
config_shell_usart(void)
{
  unsigned long baud = SHELL_DEFAULT_BAUD_RATE;
  uint16_t baud_setting = (F_CPU / (8 * baud)) - 1;

  // try u2x mode first
  UCSR0A      = _BV(U2X0);

  if (((F_CPU == 16000000UL) && (baud == 57600)) || (baud_setting >4095))
  {
    UCSR0A        = 0;
    baud_setting  = (F_CPU / (16 * baud)) - 1;
  }

  UBRR0H      = baud_setting >> 8;
  UBRR0L      = baud_setting;

  UCSR0C      = _BV(UCSZ00) | _BV(UCSZ01);                  // 8 bit data, no parity, 1 stop-bit
  UCSR0B      = _BV(RXEN0)  | _BV(TXEN0) | _BV(RXCIE0);
}

////////////////////////////////////////////////////////////////////////////////
//
// shell command handlers
//
////////////////////////////////////////////////////////////////////////////////
static void
shell_command_help(int argc, const char** argv)
{
  size_t i;

  shell_printf("\r\n");

  for(i = 0; i < sizeof(_commands)/sizeof(ShellCommand); i++)
  {
    shell_printf("%-10s: ", _commands[i].command);
    shell_printf("%s\r\n", _commands[i].description);
  }
}

static void
shell_command_version(int argc, const char** argv)
{
  shell_printf("\r\n");
  shell_printf("%s\r\n", VERSION);
}

static void
shell_command_pwm(int argc, const char** argv)
{
  uint8_t   chnl;
  uint16_t  duty;

  if(argc != 3)
  {
    shell_printf("Invalid argument\r\n");
    shell_printf("%s <chn> <duty in us>\r\n", argv[0]);
    return;
  }

  chnl = (uint8_t)atoi(argv[1]);
  duty = (uint16_t)atoi(argv[2]);

  pwm_control(chnl, duty);

  shell_printf("set channel %d to %d us\r\n", chnl, duty);
}

static void
shell_command_spi_test(int argc, const char** argv)
{
  extern volatile uint8_t _spi_test_cnt;

  shell_printf("spi test cnt: %d\r\n", _spi_test_cnt);
}

static void
shell_command_baro(int argc, const char** argv)
{
  extern volatile int32_t    _baro_press;
  extern volatile int16_t    _baro_temp;
  extern volatile uint32_t   _baro_loop;

  shell_printf("press: %ld\r\n", _baro_press);
  shell_printf("temp : %d\r\n", _baro_temp);
  shell_printf("loop : %d\r\n", _baro_loop);
}

static void
shell_command_adc(int argc, const char** argv)
{
  extern volatile uint16_t       _adc_values[4];
  uint8_t   i;

  for(i = 0; i < 4; i++)
  {
    shell_printf("%d : %u\r\n", i, _adc_values[i]);
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// shell core
//
////////////////////////////////////////////////////////////////////////////////
static void
shell_execute_command(char* cmd)
{
  static const char*    argv[SHELL_COMMAND_MAX_ARGS];
  int                   argc = 0;
  size_t                i;
  char                  *s, *t;

  while((s = strtok_r(argc  == 0 ? cmd : NULL, " \t", &t)) != NULL)
  {
    if(argc >= SHELL_COMMAND_MAX_ARGS)
    {
      shell_printf("\r\nError: too many arguments\r\n");
      return;
    }
    argv[argc++] = s;
  }

  if(argc == 0)
  {
    return;
  }

  for(i = 0; i < sizeof(_commands)/sizeof(ShellCommand); i++)
  {
    if(strcmp(_commands[i].command, argv[0]) == 0)
    {
      shell_printf("\r\nExecuting %s\r\n", argv[0]);
      _commands[i].handler(argc, argv);
      return;
    }
  }
  shell_printf("%s", "\r\nUnknown Command: ");
  shell_printf("%s", argv[0]);
  shell_printf("%s", "\r\n");
}

////////////////////////////////////////////////////////////////////////////////
//
// shell task
//
////////////////////////////////////////////////////////////////////////////////
static void
shell_task(void* pvParameters)
{
  uint8_t     b;

  while(1)
  {
    if(xQueueReceive(_rx_q, &b, portMAX_DELAY) == pdPASS)
    {
      if(b != '\r' && _cmd_buffer_ndx < SHELL_MAX_COMMAND_LEN)
      {
        if(b == '\b' || b == 0x7f)
        {
          if(_cmd_buffer_ndx > 0)
          {
            shell_printf("%c%c%c", b, 0x20, b);
            _cmd_buffer_ndx--;
          }
        }
        else
        {
          shell_send_data(b);
          _cmd_buffer[_cmd_buffer_ndx++] = b;
        }
      }
      else if(b == '\r')
      {
        _cmd_buffer[_cmd_buffer_ndx++] = '\0';

        shell_execute_command((char*)_cmd_buffer);

        _cmd_buffer_ndx = 0;
        shell_prompt();
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces 
//
////////////////////////////////////////////////////////////////////////////////
void
shell_init(void)
{
  static uint8_t        _tsk_stack[128];
  static StaticTask_t   _tsk_buffer;
  static uint8_t        _rx_q_buffer[8];
  static uint8_t        _tx_q_buffer[8];

  static StaticQueue_t         _rx_q_struct;
  static StaticQueue_t         _tx_q_struct;

  _rx_q = xQueueCreateStatic(8, sizeof(uint8_t), _rx_q_buffer, &_rx_q_struct);
  _tx_q = xQueueCreateStatic(8, sizeof(uint8_t), _tx_q_buffer, &_tx_q_struct);
  
	config_shell_usart();

  xTaskCreateStatic(shell_task,
      "Shell",
      128,
      NULL,
      tskIDLE_PRIORITY + 1,
      _tsk_stack,
      &_tsk_buffer);
}
