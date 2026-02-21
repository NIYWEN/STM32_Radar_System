//ÂÌµÆ    PC13
//ºìµÆ    PA15

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//ÈÃA15¿ÉÒÔµ±ÆÕÍ¨GPIO¿Ú
    
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure2);
    
	GPIO_SetBits(GPIOA,GPIO_Pin_15);
    GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void LED1_ON(void)//ÂÌµÆ
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void LED1_Turn(void)
{
	LED1_ON();
    Delay_s(2);
    LED1_OFF();
}

void LED2_ON(void)//ºìµÆ
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
}

void LED2_Turn(void)
{
	LED2_ON();
    Delay_s(2);
    LED2_OFF();
    Delay_s(2);
}
