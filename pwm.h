#ifndef __PWM_DEF_H__
#define __PWM_DEF_H__

extern void pwm_init(void);
extern void pwm_control(uint8_t chnl, uint16_t duty_usec);

#endif //!__PWM_DEF_H__
