#include <stm32f10x.h>
#include <stdint.h>

#define SYSCLOCK 72000000U

__IO uint32_t SysTick_CNT = 0;

void SysTick_Init(void)
{
	MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, SYSCLOCK / 1000 - 1);
	CLEAR_BIT(SysTick->VAL, SysTick_VAL_CURRENT_Msk);
	SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

void SysTick_Handler(void)
{
	if (SysTick_CNT > 0)
		SysTick_CNT--;
}

void delay_ms(uint32_t ms)
{
	MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, SYSCLOCK / 1000 - 1);
	SysTick_CNT = ms;
	while (SysTick_CNT)
	{
	}
}

void usleep(uint32_t n)
{
	uint32_t i, j;
	for (i = 1; i <= (n / 10); i++)
		for (j = 1; j <= 66; j++)
			; /* CPU clk =72MHz */
	asm("NOP");
}