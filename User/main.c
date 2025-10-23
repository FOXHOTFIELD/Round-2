#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "oled.h"
#include "Claim.h"
#include "Motor.h"
#include "Serial.h"

enum Motor_Mode Motor1_Mode = Motor_Mode_break;
enum Motor_Mode Motor2_Mode = Motor_Mode_break;

volatile int16_t Motor1_Speed;
volatile int16_t Motor2_Speed;

volatile float Target, Actual, Out;			//目标值，实际值，输出值
volatile float Kp= 1, Ki = 0, Kd = 0;					//比例项，积分项，微分项的权重
volatile float Error0, Error1, Error2;		//本次误差，上次误差，上上次误差

int flag = 0, count = 0;
int main()
{
    Serial_Init();
    TIM1_Init();
	OLED_Init();
    Motor_Init();
    Motor_SetMode(1,     Motor_Mode_rvs_rotation);
    Motor1_SetPrescaler(72-1);
    //Motor1_SetCompare1(150);

    while (1)
    {

        //OLED_ShowSignedNum(3, 7, flag , 6);	//不断刷新显示编码器测得的最新速度
        OLED_ShowSignedNum(1, 7, Motor1_Speed, 5);	//不断刷新显示编码器测得的最新速度
        OLED_ShowNum(2,1,(int)Motor1_Mode, 1);
        //OLED_ShowNum(4, 1, count, 2);
        processCmd();
    }
    
}

