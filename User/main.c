#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "MyRTC.h"
#include "LCD.h"
#include "16key.h"
#include "LED.h"
#include "PWM.h"
#include "wakeup.h"

int main(void)
{
                   // 初始化屏幕    
   key_16_GPIO_Config_Init();
   LED_Init();
   PWM_Init();
   LCD_Init(); 
   LCD_ForceClear();  //清屏
    
    ShowStuNum();//学号滚动
    LCD_ShowString(3,0,"sleeping",BLACK);
    
    EXTI_ClearITPendingBit(EXTI_Line5 | EXTI_Line6 | EXTI_Line7 | EXTI_Line8 | EXTI_Line9 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14);
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);



    __WFI();//休眠
    
    if(wakeup_flag)
    {
        LCD_ForceClear();
        NVIC_DisableIRQ(EXTI9_5_IRQn);
    NVIC_DisableIRQ(EXTI15_10_IRQn);
       if(wakeup_flag)
{
    LCD_ForceClear();
    NVIC_DisableIRQ(EXTI9_5_IRQn);
    NVIC_DisableIRQ(EXTI15_10_IRQn);

    int16_t password = 12345;
    uint16_t count = 0, cinpass = 0;

    while(count < 3)
    {
        LCD_ShowString(5, 1, "enter password:", BLACK);
        
        uint16_t x = 30;  // 显示起始位置
        cinpass = 0;
        
        while(1)
        {
            int16_t key = firstKey();
            if(key == 111) break;  // 确认键
            
            if(key >= 0 && key <= 9)
            {
                cinpass = cinpass * 10 + key;
                LCD_ShowInt(x, 30, key, BLACK);
                x += 10;  // 下一个数字往后移
                Delay_ms(200);  // 简单消抖
            }
        }
        
        if(cinpass == password)
        {
            LED1_Turn();
            LCD_ForceClear();
            break;  // 登录成功
        }
        else
        {
            count++;
            LED2_Turn();
            LCD_ForceClear();
            LCD_ShowString(5, 5, "wrong, try again", BLACK);
            Delay_s(1);
            LCD_ForceClear();
        }
    }
    
    if(count >= 3) three_sound();
}
   

    }

    
   


}


