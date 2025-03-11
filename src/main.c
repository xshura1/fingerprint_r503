#include <stm32f10x.h>
#include <stdint.h>

#include "systick.h"
#include "r503.h"
#include "uart.h"

#define LED13_ON() *((uint32_t *)(PERIPH_BB_BASE + 32 * (GPIOC_BASE - PERIPH_BASE + 0xC) + 13 * 4)) = 0
#define LED13_OFF() *((uint32_t *)(PERIPH_BB_BASE + 32 * (GPIOC_BASE - PERIPH_BASE + 0xC) + 13 * 4)) = 1

int main(void)
{
	uint8_t fingerprint_is_init = 0;
	CONFIRMATION_CODE cc;

	SysTick_Init();

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN);

	GPIOC->CRH |= GPIO_CRH_MODE13_1;
	GPIOC->CRH &= ~GPIO_CRH_CNF13;

	USART1_init();

	__enable_irq();

	LED13_OFF();

	cc = verify_pwd(0xFFFFFFFF, 0);
	if (cc == CC_OK)
	{
		fingerprint_is_init = 1;
		LED13_ON();
	}

	while (1)
	{

		if (!fingerprint_is_init)
			continue;

		if (check_sensor())
		{

			PRODUCT_INFO p_info;
			cc = read_product_info(&p_info, DEFAULT_ADDR);
			if (cc != CC_OK)
			{
				continue;
			}

			SYSTEM_PARAM s_param;
			cc = read_sys_param(&s_param, DEFAULT_ADDR);
			if (cc != CC_OK)
			{
				continue;
			}

			uint16_t t_num;
			cc = template_num(DEFAULT_ADDR, &t_num);
			if (cc != CC_OK)
			{
				continue;
			}
		}
	}
}