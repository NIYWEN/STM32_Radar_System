#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "MyRTC.h"
#include "LCD.h"
#include "16key.h"
#include "LED.h"
#include "PWM.h"
#include "wakeup.h"
#include "loadingjiemian.h"
#include "Serial.h"
uint8_t alarm_active = 0;
uint32_t last_time = 0;
uint8_t led_state = 0;


void Alarm_Update(void)
{
    if(!alarm_active) 
    {
        // 确保关闭所有输出
        LED2_OFF();
        PWM_SetCompare1(0);  // 确保蜂鸣器关闭
        return;
    }
    
    uint32_t now = GetTick();
    if(now - last_time > 200)  // 200ms切换一次
    {
        last_time = now;
        led_state = !led_state;
        
        if(led_state)
        {
            LED2_ON();
            PWM_SetCompare1(50);  // 使用PWM控制蜂鸣器持续响
        }
        else
        {
            LED2_OFF();
            PWM_SetCompare1(0);   // 蜂鸣器停
        }
    }
}


uint8_t c=0;
int main(void)
{
    
    __enable_irq();  // 确保总中断是开的
     Serial_Init();
    key_16_GPIO_Config_Init();// 初始化屏幕 
   LED_Init();
   PWM_Init();
   LCD_Init();
    LCD_ForceClear();  //清屏
   

    
   //ShowStuNum();//学号滚动
    //LCD_ShowString(3,0,"sleeping",BLACK);
    
    EXTI_ClearITPendingBit(EXTI_Line5 | EXTI_Line6 | EXTI_Line7 | EXTI_Line8 | EXTI_Line9 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14);
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);



    //__WFI();//休眠
    
    if(wakeup_flag)
    {
        LCD_ForceClear();
        NVIC_DisableIRQ(EXTI9_5_IRQn);
        NVIC_DisableIRQ(EXTI15_10_IRQn);}
    
    
int16_t enemy_x = 0;
int16_t enemy_y = 0;
uint8_t enemy_active = 0;

while(1)
{
    LCD_ForceClear();
    //loading();
    
    while(1)
    {
        Alarm_Update();
        int8_t key = KEY_SCAN();
        
        // 画雷达背景
        leidatu(0,0);
        
        // 如果有敌人，画出来
        if(enemy_active)
        {
            LCD_DrawFillCircle_Radar(enemy_x - 64, enemy_y - 64, 5, RED);
            alarm_active = 1;
            Alarm_Update();
        }
        
        // 处理串口数据
        if(Serial_GetRxFlag() == 1)
        {
            c++;
     
            // 更新敌人坐标
            enemy_x = Serial_RxPacket[0];
            enemy_y = Serial_RxPacket[1];
            enemy_active = 1;
            
            // 串口回传
   
            Serial_ReplyPacket();
            Serial_SendDiscover();
            
        }
        else if(Serial_GetRxFlag() == 0&&c==0)
        {
            LCD_ShowString(45,100,"Searching.",RED);
        }
        
        if(key == 15) break;
    }
}





}
