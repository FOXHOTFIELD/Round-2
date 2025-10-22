#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void Motor_SetMode(int numMotor, enum Motor_Mode Mode);
void Motor1_SetPrescaler(uint16_t Prescaler);
void Motor1_SetCompare1(uint16_t Compare);
#endif
