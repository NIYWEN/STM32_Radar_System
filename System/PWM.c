
//蜂鸣器 PA0


//蜂鸣器      	PA0	        TIM2_CH1	需要输出PWM（不同频率），必须用通道1 (CH1)。
//电机PWM (速度)	PA8	        TIM1_CH1	需要输出PWM（调速），必须用通道1 (CH1)。TIM1是高级定时器，初始化时记得加上 TIM_CtrlPWMOutputs(TIM1, ENABLE); 。
//舵机PWM	    PA6	        TIM3_CH1

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
void PWM_Init(void)
{
    //蜂鸣器
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
    
    
    
    
    // TIM1 时钟使能（电机）
RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

// PA8 复用推挽
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStructure);

// TIM1 时基配置（和 TIM2 一样，频率固定）
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_TimeBaseStructure.TIM_Period = 100 - 1;        // ARR
TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;     // PSC
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

// TIM1 通道1 PWM1 模式
TIM_OCInitTypeDef TIM_OCInitStructure2;
TIM_OCInitStructure2.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStructure2.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure2.TIM_Pulse = 0;
TIM_OCInitStructure2.TIM_OCPolarity = TIM_OCPolarity_High;
TIM_OC1Init(TIM1, &TIM_OCInitStructure2);

// TIM1 高级定时器要额外使能输出
TIM_CtrlPWMOutputs(TIM1, ENABLE);

TIM_Cmd(TIM1, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)//蜂鸣器
{
	TIM_SetCompare1(TIM2, Compare);
}

void three_sound(void)
{
    
    GPIO_SetBits(GPIOA, GPIO_Pin_0); 

    PWM_SetCompare1(100);
    Delay_s(1);
    PWM_SetCompare1(0);
    Delay_s(1);
    
    PWM_SetCompare1(50);
    Delay_s(1);
    PWM_SetCompare1(0);
    Delay_s(1);
    
    PWM_SetCompare1(10);
    Delay_s(1);
    PWM_SetCompare1(0);
    Delay_s(1);
}

void onesound(void)
{
    PWM_SetCompare1(100);
    Delay_s(1);
    PWM_SetCompare1(0);
    Delay_s(1);
}

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~100
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare1(TIM1, Compare);		//设置CCR3的值
}
