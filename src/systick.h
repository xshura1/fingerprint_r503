#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>

void SysTick_Init(void);
void delay_ms(uint32_t time);
void usleep(uint32_t n);

#endif /*SYSTICK_H_*/
