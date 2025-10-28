#include <stdint.h>
#ifndef __CLAIM_H
#define __CLAIM_H

enum TEST{
    TEST_1, TEST_2
};

extern enum TEST test;

extern volatile int16_t Encoder_Count;					//全局变量（volatile），用于计数旋转编码器的增量值

extern volatile int g_oled_clear_request;
extern volatile int g_encoder_init;
extern volatile int g_encoder_deinit;

enum Motor_Mode{
    Motor_Mode_break,
    Motor_Mode_frd_rotation,
    Motor_Mode_rvs_rotation,
    Motor_Mode_stop
};

extern enum Motor_Mode Motor1_Mode;
extern enum Motor_Mode Motor2_Mode;

extern volatile int16_t Motor1_Speed;
extern volatile int16_t Motor2_Speed;

extern volatile float Target, Actual, Out;			//目标值，实际值，输出值
extern volatile float Kp, Ki, Kd;					//比例项，积分项，微分项的权重
extern volatile float Error0, Error1, Error2;		//本次误差，上次误差，上上次误差

extern volatile float OuterTarget, OuterActual, OuterOut;			//目标值，实际值，输出值
extern volatile float OuterKp, OuterKi, OuterKd;					//比例项，积分项，微分项的权重
extern volatile float OuterError0, OuterError1, OuterError2;		//本次误差，上次误差，上上次误差
#endif
