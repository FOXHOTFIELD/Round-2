#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "oled.h"
#include "Claim.h"
#include "Motor.h"
#include "Serial.h"
#include "Encoder.h"

volatile int g_oled_clear_request = 0;
volatile int g_encoder_init = 0;
volatile int g_encoder_deinit = 0;

enum TEST test = TEST_1;
volatile int16_t Encoder_Count = 0;					//全局变量（volatile），用于计数旋转编码器的增量值
enum Motor_Mode Motor1_Mode = Motor_Mode_break;
enum Motor_Mode Motor2_Mode = Motor_Mode_break;

volatile int16_t Motor1_Speed;
volatile int16_t Motor2_Speed;

volatile float Target, Actual, Out;			//目标值，实际值，输出值
volatile float Kp= 1, Ki = 0.1, Kd = 0;					//比例项，积分项，微分项的权重
volatile float Error0, Error1, Error2;		//本次误差，上次误差，上上次误差
volatile float OuterTarget, OuterActual, OuterOut;			//目标值，实际值，输出值
volatile float OuterKp = 1, OuterKi = 0, OuterKd = 0.1;					//比例项，积分项，微分项的权重
volatile float OuterError0, OuterError1, OuterError2;		//本次误差，上次误差，上上次误差

int flag = 0, count = 0;
int main()
{
    Serial_Init();
    TIM1_Init();
	OLED_Init();
    Motor_Init();
    Motor_SetMode(1,     Motor_Mode_stop);
    Motor1_SetPrescaler(72-1);
    ButtonInit();
    //Motor1_SetCompare1(150);

    while (1)
    {
        OLED_ShowChar(1, 1, (test == TEST_1 ? '1' : '2'));
        if(test == TEST_1)
        {            
            if(g_oled_clear_request == 1) OLED_Clear();
            g_oled_clear_request = 0;
            if(g_encoder_deinit == 1) Encoder_DeInit();
            g_encoder_init = 0;
            //OLED_ShowSignedNum(3, 7, flag , 6);	//不断刷新显示编码器测得的最新速度
            OLED_ShowSignedNum(1, 7, Motor1_Speed, 5);	//不断刷新显示编码器测得的最新速度
            OLED_ShowNum(2,1,(int)Motor1_Mode, 1);
            OLED_ShowNum(3, 1, Target, 3);
            //OLED_ShowNum(4, 1, count, 2);
            //OLED_ShowNum(4, 7, Target, 3);
            OLED_ShowFloat(2, 8, Kp, 3);
            OLED_ShowFloat(3, 8, Ki, 3);
            OLED_ShowFloat(4, 8, Kd, 3);
            processCmd();
        }else if(test == TEST_2){
            //Serial_DeInit();
            if(g_oled_clear_request == 1) OLED_Clear();
            g_oled_clear_request = 0;
            if(g_encoder_init == 1) Encoder_Init();
            g_encoder_init = 0;
            
            OLED_ShowFloat(2, 8, OuterKp, 3);
            OLED_ShowFloat(3, 8, OuterKi, 3);
            OLED_ShowFloat(4, 8, OuterKd, 3);
            processCmd();
        }
    }
    
}

