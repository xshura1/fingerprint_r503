#include <stm32f10x.h>
#include <stddef.h>
#include <stdio.h>

#include "config.h"
#include "systick.h"

#define USART_BAUDRATE (72000000 + 28800) / 57600

// USART Receiver buffer
#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256

volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_wr_index, rx_rd_index, rx_counter;
volatile uint8_t rx_overflow = 0;

volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
volatile uint16_t tx_wr_index, tx_rd_index, tx_counter;
volatile uint8_t is_tx;

void USART1_IRQHandler(void)
{
	volatile uint16_t tmp = 0;

	if (USART1->SR & USART_SR_RXNE)
	{

		if ((USART1->SR & (USART_SR_NE | USART_SR_FE | USART_SR_PE | USART_SR_ORE)) == 0) // проверяем нет ли ошибок
		{
			is_tx = 1;
			rx_buffer[rx_wr_index++] =
				(uint8_t)(USART1->DR & 0xFF); // считываем данные в буфер, инкрементируя хвост буфера
			if (rx_wr_index == RX_BUFFER_SIZE)
				rx_wr_index = 0; // идем по кругу
			if (++rx_counter == RX_BUFFER_SIZE)
			{					 // переполнение буфера
				rx_counter = 0;	 // начинаем сначала (удаляем все данные)
				rx_overflow = 1; // сообщаем о переполнении
			}
		}
		else
		{
			tmp =
				USART1->DR; // в идеале пишем здесь обработчик ошибок, в данном случае просто пропускаем ошибочный байт.
		}
	}

	if (USART1->SR & USART_SR_TXE)
	{
		if (tx_counter)
		{				  // если есть что передать
			--tx_counter; // уменьшаем количество не переданных данных
			USART1->DR = tx_buffer[tx_rd_index++];
			if (tx_rd_index == TX_BUFFER_SIZE)
				tx_rd_index = 0; // идем по кругу
		}
		else
		{ // если нечего передать, запрещаем прерывание по передачи
			USART1->CR1 &= ~USART_SR_TXE;
		}
	}
}

void USART1_init(void)
{

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // инициализируем шину для USART1
	USART1->BRR = USART_BAUDRATE;		  // бодрэйт
	USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE |
				   USART_CR1_TXEIE; // | USART_CR1_M; // USART1 ON, TX ON, RX ON, RX, TX interrupt ON
									// USART1->CR2 |= USART_CR2_STOP_1 | USART_CR2_STOP_0;

	// настройка ножки TX (PA9)
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN; // GPIOA Clock ON. Alter function clock ON
	GPIOA->CRH &= ~GPIO_CRH_CNF9;							 // Clear CNF bit 9
	GPIOA->CRH |= GPIO_CRH_CNF9_1;							 // Set CNF bit 9 to 10 - AFIO Push-Pull
	GPIOA->CRH |= GPIO_CRH_MODE9_0;							 // Set MODE bit 9 to Mode 01 = 10MHz

	// настройка ножки RX (PA10)
	GPIOA->CRH &= ~GPIO_CRH_CNF10;	// Clear CNF bit 10
	GPIOA->CRH |= GPIO_CRH_CNF10_0; // Set CNF bit 10 to 01 = HiZ
	GPIOA->CRH &= ~GPIO_CRH_MODE10; // Set MODE bit 10 to Mode 01 = 10MHz

	NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_send_data(uint8_t *data, uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		while (tx_counter == TX_BUFFER_SIZE)
			;

		USART1->CR1 &= ~USART_SR_TXE;
		if (tx_counter || (!(USART1->SR & USART_SR_TXE)))
		{
			tx_buffer[tx_wr_index++] = (*data++);
			if (tx_wr_index == TX_BUFFER_SIZE)
				tx_wr_index = 0;
			++tx_counter;
			USART1->CR1 |= USART_SR_TXE;
		}
		else
			USART1->DR = ((*data++) & (uint16_t)0xFF);
	}
}

void USART1_get_data(uint8_t *data, uint8_t *size)
{
	uint8_t b;
	*size = 0;

	while (!is_tx)
	{
	}

	if (!(USART1->SR & USART_SR_IDLE))
	{
		is_tx = 0;
		return;
	}

	while (rx_counter != 0)
	{
		b = rx_buffer[rx_rd_index++];
		if (rx_rd_index == RX_BUFFER_SIZE)
			rx_rd_index = 0;
		USART1->CR1 &= ~USART_CR1_RXNEIE;
		--rx_counter;
		USART1->CR1 |= USART_CR1_RXNEIE;
		*data = b;
		data++;
		(*size)++;
	}
}
