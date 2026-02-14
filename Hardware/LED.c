//ÂÌµÆ    PB15
//ºìµÆ    PB15

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_11 | GPIO_Pin_15);
}

void LED1_ON(void)//ÂÌµÆ
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void LED1_Turn(void)
{
	LED1_ON();
    Delay_s(2);
    LED1_OFF();
}

void LED2_ON(void)//ºìµÆ
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_11);
}

void LED2_Turn(void)
{
	LED2_ON();
    Delay_s(2);
    LED2_OFF();
}
