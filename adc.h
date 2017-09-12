#ifndef __ACD_DEF_H__
#define __ACD_DEF_H__

typedef enum
{
  ADCRef_ARef     = 0x00,
  ADCRef_AVCC     = 0x01,
  ADCRef_1V1      = 0x03,
} ADCRef;

typedef enum
{
  ADCPrescaler_DIV_2    = 0x01,
  ADCPrescaler_DIV_4    = 0x02,
  ADCPrescaler_DIV_8    = 0x03,
  ADCPrescaler_DIV_16   = 0x04,
  ADCPrescaler_DIV_32   = 0x05,
  ADCPrescaler_DIV_64   = 0x06,
  ADCPrescaler_DIV_128  = 0x07,
} ADCPrescaler;

typedef uint16_t adcsample_t;
typedef uint8_t adc_channels_num_t;

typedef void (*adc_callback_t)(adc_channels_num_t chnl, adcsample_t sample);

extern void adc_init(ADCRef ref, ADCPrescaler prescaler);
extern void adc_start_conversion(adc_channels_num_t chnl, adc_callback_t callback);

#endif //!__ACD_DEF_H__
