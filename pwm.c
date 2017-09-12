#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

////////////////////////////////////////////////////////////////////////////////
//
// private definitions
//
////////////////////////////////////////////////////////////////////////////////
#define PWM_CNT_TICKS_1_US    2     // at 2 Mhz, 2 ticks per 1 us.
#define US_TO_CNT_TICKS(us)   ((us) * PWM_CNT_TICKS_1_US - 1)

#define PWM_DUTY_1_MS         US_TO_CNT_TICKS(1000)

//
// F = F_CPU / N(1 + TOP)
// 400 Hz = 16 MHz / 8 ( 1 + TOP) = 2 MHz /(1 + TOP)
// TOP = 2 MHz / 400 Hz - 1
//     = 5000 - 1
//     = 2500 * PWM_CNT_TICKS_1_US - 
#define PWM_PERIOD_2_5_MS     US_TO_CNT_TICKS(2500)


////////////////////////////////////////////////////////////////////////////////
//
// pwm configuration 
//
////////////////////////////////////////////////////////////////////////////////
static void
pwm_init_config(void)
{
  //
  // system running at 16 MHz 
  // With PWM clock divider with 8,
  // PWM clock is 2 MHz.
  //
  // With 2 MHz PWM Clock,
  // The goal here is
  //
  // a) 400 Hz PWM Frequency, that is, period is 2.5 ms
  // b) duty period between 0.5 ms and 2 ms.
  //
  // To achieve this, I'll use TC1 TC in fast PWM mode.
  //
  // In Fast PWM mode 14,
  // ICR1 is TOP value and we get two PWM channels (OCR1A, OCR1B),
  //
  // And in non-inverting compare match mode, channels  go high at 0,
  // stay high until counter is less then OCR1n, and go low after match.
  //


  //
  // reset counter
  //
  TCNT1   = 0;

  // initialize each channel to 1ms duty cycle
  OCR1A   = PWM_DUTY_1_MS;
  OCR1B   = PWM_DUTY_1_MS;

  // initialize TOP value so period becomes 2.5 ms
  ICR1    = PWM_PERIOD_2_5_MS;

  // setup TC mode and start counting by providing clock.
  // TCCR1A/TCCR1B
  //
  // Compare Output Mode for Fast PWM with ICR1 as TOP
  //
  // Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
  // COM1A1 : 1
  // COM1A0 : 0
  // COM1B1 : 1
  // COM1B0 : 0
  //
  //            WGM13  WGM12   WGM11   WGM10
  //Mode 14 :     1     1       1         0
  //
  // ICNC1  : Don't care
  // ICES1  : Don't care
  //
  // Clock Divider :  8 Divider
  //    CS12    CS11    CS10
  //      0       1       0
  //
  TCCR1A    = (_BV(COM1A1) | _BV(COM1B1) | _BV(WGM11));
  TCCR1B    = (_BV(WGM13)  | _BV(WGM12)  | _BV(CS11));

}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces 
//
////////////////////////////////////////////////////////////////////////////////
void
pwm_init(void)
{
  pwm_init_config();

  // PB1/PB2 are OC1x output
  //
  DDRB |= (_BV(PB1) | _BV(PB2));
}

void
pwm_control(uint8_t chnl, uint16_t duty_usec)
{
  uint16_t    ticks;
  // 1 us = 2 ticks

  ticks = US_TO_CNT_TICKS(duty_usec);

  switch(chnl)
  {
  case 0:
    OCR1A   = ticks;
    break;

  case 1:
    OCR1B   = ticks;
    break;
  }
}
