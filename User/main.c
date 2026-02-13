#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MyRTC.h"
#include "LCD.h"

int main(void)
{
    LCD_Init();                 // 初始化屏幕         
    
    LCD_ForceClear();  //清屏
    
    /* 画雷达背景
    LCD_DrawCircle(64, 64, 30, WHITE);      // 外圈
    LCD_DrawLine(34, 64, 94, 64, WHITE);    // 横轴
    LCD_DrawLine(64, 34, 64, 94, WHITE);    // 竖轴
    LCD_DrawFillCircle(64, 64, 2, RED);   // 中心点*/
    
   // LCD_ShowNum(0, 20, 12345, GREEN);


    
    
 
    LCD_Init();
    LCD_Clear(BLACK);
    
    /* 全部在屏幕范围内 (0-120)
    LCD_ShowString(0, 0, "Hello STM32!", GREEN);
    LCD_ShowString(0, 16, "Radar System", YELLOW);   // 16
    LCD_ShowString(0, 32, "LCD 1.44 SPI", BLUE);     // 32
    
    LCD_ShowString(0, 48, "Int:", BLACK);            // 48
    LCD_ShowInt(40, 48, 12345, BLACK);
    
    LCD_ShowString(0, 64, "Hex:", BLACK);            // 64
    LCD_ShowInt(48, 64, 0xABCD, BLACK);
    
    LCD_ShowString(0, 80, "Float:", BLACK);          // 80
    LCD_ShowFloat(56, 80, 3.14159f, 2, BLACK);
    
    char buf[20];
    sprintf(buf, "E:%d,%d", 10, -5);
    LCD_ShowString(0, 96, buf, RED);                 // 96
    
    sprintf(buf, "Temp:%dC", 28);
    LCD_ShowString(0, 112, buf, BLUE);               // 112
    
    while(1)
    {
        // 闪烁测试也移到可见区域
        LCD_ShowInt(80, 48, 12345, RED);
        Delay_ms(500);
        LCD_ShowInt(80, 48, 12345, GREEN);
        Delay_ms(500); 
    }*/
    ShowStuNum();
}


