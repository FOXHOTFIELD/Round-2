#include "stm32f10x.h"
#include "Claim.h"

void Motor_Init(void)
{
    /*电机1初始化*/
    
        /*pwmInit*/

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_IS;
    GPIO_IS.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_IS.GPIO_Pin = GPIO_Pin_0;                  //PWMA
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_IS);

    TIM_TimeBaseInitTypeDef TIM_TBIS;
    TIM_TBIS.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TBIS.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TBIS.TIM_Period = 100 - 1;
    TIM_TBIS.TIM_Prescaler = 720 - 1;
    TIM_TBIS.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TBIS);

    TIM_OCInitTypeDef TIM_OCIS;
    TIM_OCStructInit(&TIM_OCIS);

    TIM_OCIS.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCIS.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCIS.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCIS.TIM_Pulse = 0;
    TIM_OC1Init(TIM2, &TIM_OCIS);

    TIM_Cmd(TIM2, ENABLE);

        /*电机模式输入*/

    GPIO_IS.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_IS.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;     //AIN1 AIN2
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_IS);
    GPIO_SetBits(GPIOA, GPIO_Pin_11);
    GPIO_SetBits(GPIOA, GPIO_Pin_12);                 //初始化均为高电平 制动模式

}

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~100
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void Motor1_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM2, Compare);		//设置CCR1的值
}

/**
  * 函    数：PWM设置PSC
  * 参    数：Prescaler 要写入的PSC的值，范围：0~65535
  * 返 回 值：无
  * 注意事项：PSC和ARR共同决定频率，此函数仅设置PSC的值，并不直接是频率
  *           频率Freq = CK_PSC / (PSC + 1) / (ARR + 1)
  */
void Motor1_SetPrescaler(uint16_t Prescaler)
{
	TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);		//设置PSC的值
}

void Motor_SetMode(int numMotor, enum Motor_Mode Mode)
{
    if(numMotor == 1){
        switch (Mode)
        {
        case Motor_Mode_break:
            GPIO_SetBits(GPIOA, GPIO_Pin_11);
            GPIO_SetBits(GPIOA, GPIO_Pin_12);
            break;
        case Motor_Mode_stop:
            GPIO_ResetBits(GPIOA, GPIO_Pin_11);
            GPIO_ResetBits(GPIOA, GPIO_Pin_12);
            break;
        case Motor_Mode_frd_rotation:
            GPIO_SetBits(GPIOA, GPIO_Pin_11);
            GPIO_ResetBits(GPIOA, GPIO_Pin_12);
            break;
        case Motor_Mode_rvs_rotation:
            GPIO_ResetBits(GPIOA, GPIO_Pin_11);
            GPIO_SetBits(GPIOA, GPIO_Pin_12);
            break;
        default:
            break;
        }
    }
}
