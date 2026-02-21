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
#include "Motor.h"
#include "Servo.h"
#include "MPU6050.h"
#include "MyI2C.h"
#include <math.h>
#include <stdlib.h>
#include "TEMP.h"
uint8_t alarm_active = 0;
uint32_t last_time = 0;
uint8_t led_state = 0;
int16_t AX, AY, AZ;			//定义用于存放各个数据的变量
int16_t M_AX, M_AY, M_AZ;   //加速度计的阈值
uint8_t tilt_active = 0;           // 倾倒状态标志
uint32_t tilt_last_time = 0;        // 舵机摆动计时
uint8_t tilt_servo_pos = 0;         // 舵机摆动位置
uint32_t beep_last_time = 0;        // 蜂鸣器渐响计时
uint8_t beep_duty = 0;              // 蜂鸣器占空比
uint16_t tilt_threshold = 15000;    // 倾倒阈值

float tem;
float high_tem=30;
uint8_t tem_active=0;

void Temp_Check(void)
{
    if(tem>high_tem)
    {
        if(!tem_active)
        {
            tem_active=1;
             Serial_SendString("Overheating!\r\n");
            
            // 屏幕显示倾倒状态
            LCD_ShowString(5, 55, "Overheating!", RED);
            // 电机停止
            Motor_SetSpeed(0);
            Servo_SetAngle(0);
        }
        // 倾倒状态下的处理
        if(tem_active)
        {
            // 红灯常亮
            LED2_ON();
            
            // 蜂鸣器声音
            PWM_SetCompare1(100);
            
        }
    }
    else  // 没有倾倒
    {
        if(tem_active)  // 刚从倾倒状态恢复
        {
            tem_active = 0;
            
            // 关红灯
            LED2_OFF();
            
            // 关蜂鸣器
            PWM_SetCompare1(0);
            
        }
    }
}
            



void Alarm_Update(void)
{
    if(!alarm_active) 
    {
        LED2_OFF();
        PWM_SetCompare1(0);
        return;
    }
    
    uint32_t now = GetTick();
    
    // 第一次进入，直接亮
    if(last_time == 0)
    {
        last_time = now;
        led_state = 1;
        LED2_ON();
        PWM_SetCompare1(50);
        return;
    }
    
    if(now - last_time > 200)
    {
        last_time = now;
        led_state = !led_state;
        
        if(led_state)
        {
            LED2_ON();
            PWM_SetCompare1(50);
        }
        else
        {
            LED2_OFF();
            PWM_SetCompare1(0);
        }
    }
}

// 倾倒检测与处理函数
void Tilt_Check(void)
{
    // 计算合加速度
    int32_t acc_xy = (int32_t)AX * AX + (int32_t)AY * AY;
    uint32_t now = GetTick();
    
    if(acc_xy > tilt_threshold)  // 如果超过阈值，认为倾倒
    {
        if(!tilt_active)  // 刚进入倾倒状态
        {
            tilt_active = 1;
            beep_duty = 0;
            
            // 串口发送倾倒状态
            Serial_SendString("Tilt detected!\r\n");
            
            // 屏幕显示倾倒状态
            LCD_ShowString(5, 55, "Tilt!!!", RED);
            
            // 电机停止
            Motor_SetSpeed(0);
            
            // 关掉敌人警报（因为要进入倾倒状态了）
            alarm_active = 0;
        }
        
        // 倾倒状态下的处理
        if(tilt_active)
        {
            // 红灯常亮
            LED2_ON();
            
            // 蜂鸣器声音逐渐变大（PWM占空比逐渐增加）
            if(now - beep_last_time > 50)
            {
                beep_last_time = now;
                beep_duty += 2;
                if(beep_duty > 100) beep_duty = 100;
                PWM_SetCompare1(beep_duty);
            }
            
            // 舵机往复摆动
            if(now - tilt_last_time > 50)
            {
                tilt_last_time = now;
                tilt_servo_pos = !tilt_servo_pos;
                
                // 在两个极限位置来回摆动
                if(tilt_servo_pos)
                    Servo_SetAngle(30);   // 摆到30度
                else
                    Servo_SetAngle(150);  // 摆到150度
            }
        }
    }
    else  // 没有倾倒
    {
        if(tilt_active)  // 刚从倾倒状态恢复
        {
            tilt_active = 0;
            beep_duty = 0;
            
            // 串口发送恢复状态
            Serial_SendString("Tilt recovered\r\n");
            
            // 关红灯
            LED2_OFF();
            
            // 关蜂鸣器
            PWM_SetCompare1(0);
            
            // 舵机复位到中间
            Servo_SetAngle(90);
        }
    }
}

uint8_t c=0;
uint8_t menu=0;// 0=不在菜单, 1=在菜单, 2=在修改数值
int main(void)
{
    
    __enable_irq();  // 确保总中断是开的
     Serial_Init();
    key_16_GPIO_Config_Init();// 初始化屏幕 
   LED_Init();
   PWM_Init();
    Motor_Init();		//直流电机初始化
    Servo_Init();
    	 MPU6050_Init();		//MPU6050初始化
    
    MPU6050_ReadReg(0x75);

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
         tilt_active = 0;
    beep_duty = 0;
    }
    
    
int16_t enemy_x = 0;
int16_t enemy_y = 0;
uint8_t enemy_active = 0;
float distance=sqrtf(abs(enemy_x-68)*abs(enemy_x-68)+abs(enemy_y-68)*abs(enemy_y-68));       
uint8_t menu = 0;                // 0=雷达, 1=菜单, 2=修改移动距离, 3=修改转速
uint8_t selected_item = 0;       // 0=第一项, 1=第二项
uint16_t movement_step = 10;     // 单位距离，默认10
uint16_t motor_speed = 100;      // 电机转速

uint8_t n;    
    
while(1)
{
    LCD_ForceClear();

    loading();
    
   while(1)
    {
        MPU6050_GetData(&AX, &AY, &AZ);		//获取MPU6050的数据
       
        
 

    tem= DS18B20_GetTemp();
        
        Tilt_Check();// 执行倾倒检测
    Temp_Check();//执行温度检测
        int8_t key = KEY_SCAN();
        
        if(tilt_active)
        {
            
            continue;  // 跳过下面的所有处理
        }
        if(tem_active)
        {
            
            continue;  // 跳过下面的所有处理
        }
        
        
        if(menu == 0)
        {
            
            if(key == 13)         // 按菜单键
            {
                menu = 1;    // 进入菜单
                LCD_ForceClear();
                LCD_ShowString(5,5,"1.unit movement",BLACK);
                LCD_ShowString(5,20,"2.motor speed",BLACK);
                LCD_ShowString(5,35,"3.accelerated speed",BLACK);
            }
            
            else{
                // 画雷达背景
                leidatu(0,0);
                
                MPU6050_GetData(&AX, &AY, &AZ);		//获取MPU6050的数据
        
                LCD_ShowInt(90,70,AX,YELLOW);//写加速度
                LCD_ShowInt(90,85,AY,YELLOW);
                LCD_ShowInt(90,100,AZ,YELLOW);
                LCD_ShowFloat(90,55,tem,1,YELLOW);
               
                // 如果有敌人，画出来
                if(enemy_active)
                {
                    LCD_DrawFillCircle_Radar(enemy_x - 64, enemy_y - 64, 5, RED);
                    alarm_active = 1;
                    Alarm_Update();
                    LCD_ShowString(5,100,"discover the enemy",RED);
                    
                    if(key == 14)  // 开火键
                    {
    // 实时计算距离
    float dx = enemy_x - 68;
    float dy = enemy_y - 68;
    float distance = sqrtf(dx*dx + dy*dy);
    
    if(distance <= movement_step && enemy_active)  // 距离小于阈值且有敌人
    {
        enemy_active = 0;           // 敌人消失
        LED1_ON();                  // 绿灯亮
        PWM_SetCompare1(100);       // 蜂鸣器响
        Serial_SendString("Enemy eliminated!\r\n");
        Delay_s(2);                  // 等2秒
        LED1_OFF();                 // 绿灯灭
        PWM_SetCompare1(0);         // 蜂鸣器停
        
        // 清空雷达上的敌人
        leidatu(0,0);
    }
}
                }
                
                // 处理串口数据
                if(Serial_GetRxFlag() == 1)
                {
                    c++;
                    enemy_x = Serial_RxPacket[0];
                    enemy_y = Serial_RxPacket[1];
                    enemy_active = 1;
                    alarm_active = 1;
                    last_time = 0;  // 让警报立即启动

                    Serial_ReplyPacket();
                   Serial_SendDiscover();
                }
                else if(Serial_GetRxFlag() == 0 && c == 0)
                {
                    LCD_ShowString(45,100,"Searching.",RED);
                }
                
                if(key == 15) break;
                
                
                if(key==16)  // 上移
{
    alarm_active = 0;
    // 计算移动时间
    float move_time = (float)movement_step / motor_speed;  // 单位：秒
    uint16_t steps = (uint16_t)(move_time * 20);  // 每步50ms，共steps步
    
    Motor_SetSpeed(motor_speed);
    
    for(n=0; n<steps; n++)
    {
        enemy_y += 1;  // 每次移动1个单位
        
        // 更新雷达显示
        leidatu(enemy_x, enemy_y);
        LCD_DrawFillCircle_Radar(enemy_x - 64, enemy_y - 64, 5, RED);
        // 舵机瞄准当前敌人位置
        float rad = atan2(enemy_y - 64, enemy_x - 64);
        float deg = rad * 180.0f / 3.14159f;
        Servo_SetAngle(deg);
        
        Delay_ms(50);
    }
    
    Motor_SetSpeed(0);
}
                    
if(key==12)  // 下移
{
    alarm_active = 0;
    float move_time = (float)movement_step / motor_speed;
    uint16_t steps = (uint16_t)(move_time * 20);
    
    Motor_SetSpeed(motor_speed);
    
    for(n=0; n<steps; n++)
    {
        enemy_y -= 1;
        leidatu(enemy_x, enemy_y);
        
        float rad = atan2(enemy_y - 64, enemy_x - 64);
        float deg = rad * 180.0f / 3.14159f;
        Servo_SetAngle(deg);
        
        Delay_ms(50);
    }
    
    Motor_SetSpeed(0);
}
                    
if(key==4)  // 左移
{
    alarm_active = 0;
    float move_time = (float)movement_step / motor_speed;
    uint16_t steps = (uint16_t)(move_time * 20);
    
    Motor_SetSpeed(motor_speed);
    
    for(n=0; n<steps; n++)
    {
        enemy_x -= 1;
        leidatu(enemy_x, enemy_y);
        LCD_DrawFillCircle_Radar(enemy_x - 64, enemy_y - 64, 5, RED);
        float rad = atan2(enemy_y - 64, enemy_x - 64);
        float deg = rad * 180.0f / 3.14159f;
        Servo_SetAngle(deg);
        
        Delay_ms(50);
    }
    
    Motor_SetSpeed(0);
}
                    
if(key==8)  // 敌人右移
{
    alarm_active = 0;
    float move_time = (float)movement_step / motor_speed;
    uint16_t steps = (uint16_t)(move_time * 20);
    
    Motor_SetSpeed(motor_speed);
    
    for(n=0; n<steps; n++)
    {
        enemy_x += 1;
        leidatu(enemy_x, enemy_y);
        LCD_DrawFillCircle_Radar(enemy_x - 64, enemy_y - 64, 5, RED);
        float rad = atan2(enemy_y - 64, enemy_x - 64);
        float deg = rad * 180.0f / 3.14159f;
        Servo_SetAngle(deg);
        
        Delay_ms(50);
    }
    
    Motor_SetSpeed(0);
}
                
                    
                
                    
                            
            }
        }//menu==0


        else if(menu == 1)
        {
            //进入修改移动距离
            if(key == 1)
            {
                menu = 2;
                selected_item = 0;
                LCD_ForceClear();
                LCD_ShowString(5,5,"Set movement:",BLACK);
                LCD_ShowString(5,20,"Current:",BLACK);
                LCD_ShowInt(80,20,movement_step,BLACK);
            }
            //进入修改电机转速
            else if(key == 2)
            {
                menu = 3;
                selected_item = 1;
                LCD_ForceClear();
                LCD_ShowString(5,5,"Set speed:",BLACK);
                LCD_ShowString(5,20,"Current:",BLACK);
                LCD_ShowInt(80,20,motor_speed,BLACK);
            }
            //退出菜单
            else if(key == 15)
            {
                menu = 0;
                LCD_ForceClear();
            }
        }


        else if(menu == 2)
        {

            if(key == 12)
            {
                LCD_ShowInt(80,20,movement_step,WHITE);
                movement_step++;
                LCD_ShowInt(80,20,movement_step,BLACK);
                
            }

            else if(key == 16)
            {
                 LCD_ShowInt(80,20,movement_step,WHITE);
                if(movement_step > 1) movement_step--;
                LCD_ShowInt(80,20,movement_step,BLACK);
            }

            else if(key == 11)
            {
                menu = 1;
                LCD_ForceClear();
                LCD_ShowString(5,5,"1.unit movement",BLACK);
                LCD_ShowString(5,20,"2.motor speed",BLACK);
                LCD_ShowString(5,35,"3.accelerated speed",BLACK);
            }

            else if(key == 15)
            {
                menu = 1;
                LCD_ForceClear();
                LCD_ShowString(5,5,"1.unit movement",BLACK);
                LCD_ShowString(5,20,"2.motor speed",BLACK);
                LCD_ShowString(5,35,"3.accelerated speed",BLACK);
            }
        }


        else if(menu == 3)
        {

            if(key == 12)
            {
                LCD_ShowInt(80,20,motor_speed,WHITE);
                motor_speed += 10;
                if(motor_speed > 1000) motor_speed = 1000;
                LCD_ShowInt(80,20,motor_speed,BLACK);
            }

            else if(key == 16)
            {
                LCD_ShowInt(80,20,motor_speed,WHITE);
                if(motor_speed > 10) motor_speed -= 10;
                LCD_ShowInt(80,20,motor_speed,BLACK);
            }

            else if(key == 11)
            {
                menu = 1;
                LCD_ForceClear();
                LCD_ShowString(5,5,"1.unit movement",BLACK);
                LCD_ShowString(5,20,"2.motor speed",BLACK);
                LCD_ShowString(5,35,"3.accelerated speed",BLACK);
            }

            else if(key == 15)
            {
                menu = 1;
                LCD_ForceClear();
                LCD_ShowString(5,5,"1.unit movement",BLACK);
                LCD_ShowString(5,20,"2.motor speed",BLACK);
                LCD_ShowString(5,35,"3.accelerated speed",BLACK);
            }
        }
        
    }//内层while
}
	
	
}



