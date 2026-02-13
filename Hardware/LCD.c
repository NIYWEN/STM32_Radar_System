#include "stm32f10x.h"                  // Device header
#include "LCD.h"
#include "Delay.h"
#include "LCD_Font.h"
#include <math.h>
#include <stdlib.h>  // 提供 abs()
#include "LCD_Font.h"
#include <stdio.h>


/* SPI写一个字节 */
static void SPI_WriteByte(u8 data)
{
    u8 i;
    for(i = 0; i < 8; i++)
    {
        if(data & 0x80) LCD_SDA_H;
        else LCD_SDA_L;
        
        LCD_SCL_L;
        LCD_SCL_H;
        data <<= 1;
    }
}

/* 写命令 */
void LCD_WriteCmd(u8 cmd)
{
    LCD_CS_L;
    LCD_RS_L;           // 命令模式
    SPI_WriteByte(cmd);
    LCD_CS_H;
}

/* 写8位数据 */
void LCD_WriteData(u8 data)
{
    LCD_CS_L;
    LCD_RS_H;           // 数据模式
    SPI_WriteByte(data);
    LCD_CS_H;
}

/* 写16位数据（颜色）*/
void LCD_WriteData_16Bit(u16 data)
{
    LCD_CS_L;
    LCD_RS_H;
    SPI_WriteByte(data >> 8);   // 高8位
    SPI_WriteByte(data);        // 低8位
    LCD_CS_H;
}

/* GPIO初始化 */
void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    
    // PA4(CS), PA5(SCL), PA7(SDA)
    GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN | LCD_SCL_PIN | LCD_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PB0(RST), PB1(RS), PB10(BL)
    GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN | LCD_RS_PIN | LCD_BL_PIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始状态
    LCD_CS_H;
    LCD_SCL_H;
    LCD_SDA_H;
    LCD_RST_H;
    LCD_RS_H;
    LCD_BL_H;
}

/* LCD初始化序列（ST7735S 1.44寸）*/
void LCD_Init(void)
{
    LCD_GPIO_Init();
    
    // 硬件复位
    LCD_RST_L;
    Delay_ms(10);
    LCD_RST_H;
    Delay_ms(120);
    
    // 初始化命令序列（标准ST7735S 1.44寸）
    LCD_WriteCmd(0x11);     // 退出睡眠
    Delay_ms(120);
    
    LCD_WriteCmd(0x36);     // 内存数据访问控制
    LCD_WriteData(0x08);    // 根据你的屏方向调整
    
    LCD_WriteCmd(0x3A);     // 接口像素格式
    LCD_WriteData(0x05);    // 16位色
    
    LCD_WriteCmd(0x29);     // 开启显示
    Delay_ms(10);
    
    LCD_Clear(BLACK);       // 清屏为黑色
    LCD_BL_H;               // 开背光
}

/* 设置光标位置 */
void LCD_SetCursor(u16 x, u16 y)
{
    LCD_WriteCmd(0x2A);     // 列地址设置
    LCD_WriteData_16Bit(x);
    LCD_WriteData_16Bit(x);
    
    LCD_WriteCmd(0x2B);     // 行地址设置
    LCD_WriteData_16Bit(y);
    LCD_WriteData_16Bit(y);
    
    LCD_WriteCmd(0x2C);     // 写存储器
}

/* 画点 */
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    if(x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    
    LCD_SetCursor(x, y);
    LCD_WriteData_16Bit(color);
}

/* 清屏 */
void LCD_Clear(u16 color)
{
    u16 i, j;
    LCD_SetCursor(0, 0);
    
    for(i = 0; i < LCD_WIDTH; i++)
    {
        for(j = 0; j < LCD_HEIGHT; j++)
        {
            LCD_WriteData_16Bit(color);
        }
    }
}

/* 画线（Bresenham算法）*/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    s16 dx, dy, sx, sy, err, e2;
    
    dx = abs(x2 - x1);
    dy = abs(y2 - y1);
    sx = (x1 < x2) ? 1 : -1;
    sy = (y1 < y2) ? 1 : -1;
    err = dx - dy;
    
    while(1)
    {
        LCD_DrawPoint(x1, y1, color);
        if(x1 == x2 && y1 == y2) break;
        
        e2 = 2 * err;
        if(e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if(e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

/* 画圆（Bresenham算法）*/
void LCD_DrawCircle(u16 x0, u16 y0, u16 r, u16 color)
{
    s16 x = 0, y = r;
    s16 d = 3 - 2 * r;
    
    while(x <= y)
    {
        LCD_DrawPoint(x0 + x, y0 + y, color);
        LCD_DrawPoint(x0 - x, y0 + y, color);
        LCD_DrawPoint(x0 + x, y0 - y, color);
        LCD_DrawPoint(x0 - x, y0 - y, color);
        LCD_DrawPoint(x0 + y, y0 + x, color);
        LCD_DrawPoint(x0 - y, y0 + x, color);
        LCD_DrawPoint(x0 + y, y0 - x, color);
        LCD_DrawPoint(x0 - y, y0 - x, color);
        
        x++;
        if(d < 0) d += 4 * x + 6;
        else
        {
            y--;
            d += 4 * (x - y) + 10;
        }
    }
}

/* 画实心圆 */
void LCD_DrawFillCircle(u16 x0, u16 y0, u16 r, u16 color)
{
    s16 x, y;
    for(y = y0 - r; y <= y0 + r; y++)
    {
        for(x = x0 - r; x <= x0 + r; x++)
        {
            if((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r * r)
            {
                LCD_DrawPoint(x, y, color);
            }
        }
    }
}

// 暴力清屏：直接把0x0000写入每一个像素
void LCD_ForceClear(void)
{
    u16 i, j;
    
    // 设置列范围 0-127
    LCD_WriteCmd(0x2A);
    LCD_WriteData_16Bit(0);
    LCD_WriteData_16Bit(127);
    
    // 设置行范围 0-127
    LCD_WriteCmd(0x2B);
    LCD_WriteData_16Bit(0);
    LCD_WriteData_16Bit(127);
    
    // 开始写内存
    LCD_WriteCmd(0x2C);
    
    // 写入128*128个白色像素
    for(i = 0; i <128; i++)
    {
        for(j = 0; j <128; j++)
        {
            LCD_WriteData_16Bit(WHITE);
        }
    }
}



/* 显示一个字符（8x16）*/
void LCD_ShowChar(u16 x, u16 y, char c, u16 color)
{
    u8 i, j;
    u8 temp;
    
    if(c < 0x20 || c > 0x7E) c = 0x20;
    c -= 0x20;
    
    for(i = 0; i < 8; i++)  // 8列
    {
        // 上半部分（高8位）
        temp = ascii_8x16[(u8)c][i];
        for(j = 0; j < 8; j++)
        {
            if(temp & 0x01)
                LCD_DrawPoint(x + i, y + j, color);
            temp >>= 1;
        }
        
        // 下半部分（低8位）
        temp = ascii_8x16[(u8)c][i + 8];
        for(j = 0; j < 8; j++)
        {
            if(temp & 0x01)
                LCD_DrawPoint(x + i, y + j + 8, color);
            temp >>= 1;
        }
    }
}

/* 显示字符串 */
void LCD_ShowString(u16 x, u16 y, char *str, u16 color)
{
    while(*str)
    {
        LCD_ShowChar(x, y, *str++, color);
        x += 8;  // 8像素宽 + 0间隔（字库本身有间隔）
        if(x > LCD_WIDTH - 8)
        {
            x = 0;
            y += 16;
        }
    }
}

/* 显示整数 */
void LCD_ShowInt(u16 x, u16 y, s32 num, u16 color)
{
    char buf[16];
    sprintf(buf, "%d", num);
    LCD_ShowString(x, y, buf, color);
}

/* 显示浮点数 */
void LCD_ShowFloat(u16 x, u16 y, float num, u8 decimal, u16 color)
{
    char buf[16];
    
    // 处理负数
    if(num < 0)
    {
        LCD_ShowChar(x, y, '-', color);
        x += 8;
        num = -num;
    }
    
    // 整数部分
    s32 int_part = (s32)num;
    LCD_ShowInt(x, y, int_part, color);
    
    // 小数点
    x += 8 * (int_part >= 1000 ? 4 : (int_part >= 100 ? 3 : (int_part >= 10 ? 2 : 1)));
    LCD_ShowChar(x, y, '.', color);
    x += 8;
    
    // 小数部分
    float frac_part = num - int_part;
    for(u8 i = 0; i < decimal; i++)
    {
        frac_part *= 10;
        u8 digit = (u8)frac_part;
        LCD_ShowChar(x, y, '0' + digit, color);
        x += 8;
        frac_part -= digit;
    }
}


//滚动显示学号
void ShowStuNum(void)
{
    uint8_t i;
    for(i=1;i<80;i+=3)
    {
        LCD_ShowString(i,0,"8614",BLACK);
        Delay_ms(50);
        LCD_ShowString(i,0,"8614",WHITE);
    }
    LCD_ForceClear();
}