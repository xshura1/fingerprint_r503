#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void USART1_init(void);
void USART1_get_data(uint8_t *data, uint8_t *size);
void USART1_send_data(uint8_t *data, uint8_t size);

#endif /*UART_H_*/
