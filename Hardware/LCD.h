#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"                  // Device header


/* 颜色定义 */
#define WHITE   0xFFFF
#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0

/* 屏幕尺寸（根据你的屏修改，1.44寸一般是128x128）*/
#define LCD_WIDTH   128
#define LCD_HEIGHT  128
#define LCD_CENTER_X 64
#define LCD_CENTER_Y 64

/* 硬件引脚宏定义 */
#define LCD_SCL_PIN      GPIO_Pin_5
#define LCD_SDA_PIN      GPIO_Pin_7
#define LCD_CS_PIN       GPIO_Pin_4
#define LCD_RS_PIN       GPIO_Pin_1
#define LCD_RST_PIN      GPIO_Pin_0
#define LCD_BL_PIN       GPIO_Pin_10

#define LCD_SCL_PORT     GPIOA
#define LCD_SDA_PORT     GPIOA
#define LCD_CS_PORT      GPIOA
#define LCD_RS_PORT      GPIOB
#define LCD_RST_PORT     GPIOB
#define LCD_BL_PORT      GPIOB

/* 控制宏 */
#define LCD_SCL_H        LCD_SCL_PORT->BSRR = LCD_SCL_PIN
#define LCD_SCL_L        LCD_SCL_PORT->BRR  = LCD_SCL_PIN

#define LCD_SDA_H        LCD_SDA_PORT->BSRR = LCD_SDA_PIN
#define LCD_SDA_L        LCD_SDA_PORT->BRR  = LCD_SDA_PIN

#define LCD_CS_H         LCD_CS_PORT->BSRR  = LCD_CS_PIN
#define LCD_CS_L         LCD_CS_PORT->BRR   = LCD_CS_PIN

#define LCD_RS_H         LCD_RS_PORT->BSRR  = LCD_RS_PIN
#define LCD_RS_L         LCD_RS_PORT->BRR   = LCD_RS_PIN

#define LCD_RST_H        LCD_RST_PORT->BSRR = LCD_RST_PIN
#define LCD_RST_L        LCD_RST_PORT->BRR  = LCD_RST_PIN

#define LCD_BL_H         LCD_BL_PORT->BSRR  = LCD_BL_PIN
#define LCD_BL_L         LCD_BL_PORT->BRR   = LCD_BL_PIN

/* 函数声明 */
void LCD_GPIO_Init(void);
void LCD_Init(void);
void LCD_WriteCmd(u8 cmd);
void LCD_WriteData(u8 data);
void LCD_WriteData_16Bit(u16 data);
void LCD_SetCursor(u16 x, u16 y);
void LCD_DrawPoint(u16 x, u16 y, u16 color);
void LCD_Clear(u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_ForceClear(void);
void LCD_ShowFloat(u16 x, u16 y, float num, u8 decimal, u16 color);
void LCD_ShowInt(u16 x, u16 y, s32 num, u16 color);
void LCD_ShowString(u16 x, u16 y, char *str, u16 color);
void LCD_ShowChar(u16 x, u16 y, char c, u16 color);
void ShowStuNum(void);
void LCD_DrawCircle_Radar(s16 x0, s16 y0, u16 r, u16 color);
void LCD_DrawFillCircle_Radar(s16 x0, s16 y0, u16 r, u16 color);
void leidatu(int8_t x,int8_t y);//敌人坐标，画点要注意转换
void redpointmoving(uint16_t x,uint16_t y);

#endif
