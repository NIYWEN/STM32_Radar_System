#include "stm32f10x.h"                  // Device header
#include "Delay.h"

// 内部函数声明
static void DS18B20_Mode_OUT(void);
static void DS18B20_Mode_IN(void);
static uint8_t DS18B20_Reset(void);
static void DS18B20_WriteByte(uint8_t data);
static uint8_t DS18B20_ReadByte(void);

/**
  * 函    数：DS18B20 GPIO初始化
  * 参    数：无
  * 返 回 值：无
  */
void DS18B20_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出（单总线需要）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOA, GPIO_Pin_1);  // 默认拉高
}

/**
  * 函    数：设置为输出模式
  */
static void DS18B20_Mode_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * 函    数：设置为输入模式
  */
static void DS18B20_Mode_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * 函    数：复位DS18B20
  * 返 回 值：0=存在, 1=不存在
  */
static uint8_t DS18B20_Reset(void)
{
    uint8_t presence;
    
    DS18B20_Mode_OUT();
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);      // 拉低480-960us
    Delay_us(500);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);      // 释放总线
    Delay_us(60);         // 等待15-60us
    
    DS18B20_Mode_IN();
    presence = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);  // 读取存在脉冲（0=存在）
    
    Delay_us(500);        // 等待从机释放总线
    
    return presence;
}

/**
  * 函    数：写一个字节
  */
static void DS18B20_WriteByte(uint8_t data)
{
    uint8_t i;
    
    DS18B20_Mode_OUT();
    
    for(i = 0; i < 8; i++)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);      // 拉低总线
        Delay_us(2);
        
        if(data & 0x01)       // 写1
            GPIO_SetBits(GPIOA, GPIO_Pin_1);
        else                  // 写0
            GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        
        Delay_us(60);         // 保持60us
        GPIO_SetBits(GPIOA, GPIO_Pin_1);      // 释放总线
        Delay_us(2);
        
        data >>= 1;
    }
}

/**
  * 函    数：读一个字节
  */
static uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, data = 0;
    
    for(i = 0; i < 8; i++)
    {
        data >>= 1;
        
        DS18B20_Mode_OUT();
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);      // 拉低总线启动读时序
        Delay_us(2);
        GPIO_SetBits(GPIOA, GPIO_Pin_1);      // 释放总线
        Delay_us(5);
        
        DS18B20_Mode_IN();
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))      // 读取数据
            data |= 0x80;
        
        Delay_us(60);
    }
    
    return data;
}

/**
  * 函    数：读取温度（返回浮点值）
  * 参    数：无
  * 返 回 值：温度值（单位℃），例如 25.5
  */
float DS18B20_GetTemp(void)
{
    uint8_t TL, TH;
    int16_t temp_raw;
    float temperature;
    
    // 1. 复位
    DS18B20_Reset();
    
    // 2. 跳过ROM（总线上只有一个设备）
    DS18B20_WriteByte(0xCC);
    
    // 3. 启动温度转换
    DS18B20_WriteByte(0x44);
    
    // 等待转换完成（12位精度最大750ms）
    // 这里可以加个超时，简单点直接延时
    Delay_ms(800);
    
    // 4. 再次复位
    DS18B20_Reset();
    
    // 5. 跳过ROM
    DS18B20_WriteByte(0xCC);
    
    // 6. 读取暂存器
    DS18B20_WriteByte(0xBE);
    
    // 7. 读取温度值（先读低8位，再读高8位）
    TL = DS18B20_ReadByte();
    TH = DS18B20_ReadByte();
    
    // 8. 合并为16位数据
    temp_raw = (int16_t)((TH << 8) | TL);
    
    // 9. 转换为实际温度（12位分辨率，0.0625℃/LSB）
    temperature = (float)temp_raw * 0.0625f;
    
    return temperature;
}
