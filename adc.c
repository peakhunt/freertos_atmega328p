#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "adc.h"

////////////////////////////////////////////////////////////////////////////////
//
// module privates
//
////////////////////////////////////////////////////////////////////////////////
static adc_callback_t     _complete_callback = NULL;
static adc_channels_num_t _current_chnl = 0;

////////////////////////////////////////////////////////////////////////////////
//
// private utilities
//
////////////////////////////////////////////////////////////////////////////////
static inline void
adc_select_channel(adc_channels_num_t chnl)
{
  ADMUX = (ADMUX & 0xf8) | (chnl & 0x07);
}

static inline void
adc_set_adcref(ADCRef adcref)
{
  ADMUX &= ~0xc0;
  ADMUX |= (adcref << 6);
}

static inline void
adc_set_prescaler(ADCPrescaler prescaler)
{
  ADCSRA &= ~0x07;
  ADCSRA |= prescaler;
}

////////////////////////////////////////////////////////////////////////////////
//
// ADC Complete ISR
//
////////////////////////////////////////////////////////////////////////////////
ISR(ADC_vect)
{
  uint8_t low     = ADCL;
  uint8_t high    = ADCH;
  uint16_t result = (high << 8) | low;

  if(_complete_callback != NULL)
  {
    _complete_callback(_current_chnl, result);
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
adc_init(ADCRef ref, ADCPrescaler prescaler)
{
  adc_set_adcref(ref);
  adc_set_prescaler(prescaler);

  // enable adc interrupt and activate clock
  ADCSRA |= (_BV(ADIE) | _BV(ADEN));
}

void
adc_start_conversion(adc_channels_num_t chnl, adc_callback_t callback)
{
  _complete_callback  = callback;
  _current_chnl       = chnl;

  adc_select_channel(chnl);

  // start conversion
  ADCSRA |= _BV(ADSC);
}
