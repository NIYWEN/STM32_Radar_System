//4*4键盘接线

//              行1   PB5
//              行2   PB6
//              行3   PB7
//              行4   PB8

//              列1   PB9
//              列2   PB12
//              列3   PB13 
//				列4   PB14



#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LCD.h"
#include "wakeup.h"
uint8_t Key_row[1]={0xff};   //保存按键行扫描情况的状态数组
uint8_t key_Data;

/***
 *函数名：KEY_ROW_SCAN
 *功  能：按键行扫描
 *返回值：1~4，对应1~4行按键位置
 */
uint16_t KEY_ROW_SCAN(void)
{
    //读出行扫描状态
    Key_row[0] = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)<<3;//列1读取
    Key_row[0] = Key_row[0] | (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)<<2);//列2读取
    Key_row[0] = Key_row[0] | (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)<<1);//列3读取
    Key_row[0] = Key_row[0] | (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14));//列4读取
    
    if(Key_row[0] != 0x0f)         //行扫描有变化，判断该列有按键按下
    {
      Delay_ms(10);                    //消抖
      if(Key_row[0] != 0x0f)
        {   
                switch(Key_row[0])
                {
                    case 0x07:         //0111 判断为该列第1行的按键按下
                        return 1;
                    case 0x0b:         //1011 判断为该列第2行的按键按下
                        return 2;
                    case 0x0d:         //1101 判断为该列第3行的按键按下
                        return 3;
                    case 0x0e:         //1110 判断为该列第4行的按键按下
                        return 4;
                    default :
                        return 0;
                }
        }
        else return 0;
    }
    else return 0;
}

/***
 *函数名：KEY_SCAN
 *功  能：4*4按键扫描
 *返回值：0~16，对应16个按键
 */
//**********给每个按键编号************
uint16_t KEY_SCAN(void)
{    
    uint16_t Key_Num=0;       //1-16对应的按键数
    uint16_t key_row_num=0;        //行扫描结果记录
    
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);	//行1置低
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    { 
        while(KEY_ROW_SCAN() != 0);  //消抖
        Key_Num = 0 + key_row_num;
    
    }
		GPIO_SetBits(GPIOB, GPIO_Pin_5);	//行1置高
    
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);	//行2置低       
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    { 
        while(KEY_ROW_SCAN() != 0);
        Key_Num = 4 + key_row_num;
    }
    GPIO_SetBits(GPIOB, GPIO_Pin_6);	//行2置高
    
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);	//行3置低     
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    { 
        while(KEY_ROW_SCAN() != 0);
    Key_Num = 8 + key_row_num;
    }
		GPIO_SetBits(GPIOB, GPIO_Pin_7);	//行3置高
    
		GPIO_ResetBits(GPIOB, GPIO_Pin_8);	//行4置低    
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    {
        while(KEY_ROW_SCAN() != 0);
        Key_Num = 12 + key_row_num;

    }
		GPIO_SetBits(GPIOB, GPIO_Pin_8);	//行4置高  
    
    return Key_Num;
}

//第一部分：输入密码部分
int16_t firstKey(void)
{
    switch(KEY_SCAN())
    {
        case 1:return 1;
        case 2:return 2;
        case 3:return 3;
        case 4:return 4;
        case 5:return 5;
        case 6:return 6;
        case 7:return 7;
        case 8:return 8;
        case 9:return 9;
        case 10:return 0;
        case 11:return 111;
        
    }
    return -1;
}

void key_16_GPIO_Config_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	 //使能PA端口时钟使能
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;				 //PA0,1,2,3端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line =EXTI_Line5|EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9|EXTI_Line12|EXTI_Line13|EXTI_Line14;
    EXTI_InitStruct.EXTI_LineCmd =ENABLE;
    EXTI_InitStruct.EXTI_Mode =EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger =EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStruct);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel =EXTI9_5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority =1;
    NVIC_Init(&NVIC_InitStruct);
    
    NVIC_InitTypeDef NVIC_InitStruct2;
    NVIC_InitStruct2.NVIC_IRQChannel =EXTI15_10_IRQn;
    NVIC_InitStruct2.NVIC_IRQChannelCmd =ENABLE;
    NVIC_InitStruct2.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStruct2.NVIC_IRQChannelSubPriority =1;
    NVIC_Init(&NVIC_InitStruct2);
}
//主函数，采用外部8M晶振，72M系统主频，可以在void SetSysClock(void)函数中选择主频率设置



void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5|EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5|EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9);
        wakeup_flag = 1;  // 只设标志，不做别的
    }

}

void EXTI15_10_IRQHandler (void)
    
{
    if(EXTI_GetITStatus(EXTI_Line12|EXTI_Line13|EXTI_Line14) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line12|EXTI_Line13|EXTI_Line14);
        wakeup_flag = 1;  // 只设标志，不做别的
    }

}
    