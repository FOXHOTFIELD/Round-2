#include "stm32f10x.h"                  // Device header
#include "Claim.h"

// 使用 TIM4 作为编码器计数（PB6/PB7）
void Encoder_Init(void)
{
    /* 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    // GPIOB 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);     // TIM4 时钟

    /* GPIO 配置：PB6/PB7 作为输入，上拉（也可用浮空） */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 基本时钟/计数器配置 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;               // 16 位计数器满量程
    TIM_TimeBaseStructure.TIM_Prescaler = 0;                 // 不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* 输入捕获通道配置（共用配置） */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;      // 上升沿采样（编码器模式时两相可用）
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;                         // 可根据需要设置滤波

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    /* 启用编码器接口：TI1 & TI2 模式（相位计数） */
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    /* 计数器清零并启动 */
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
}

/* 获取增量并清零（返回值为 signed 16 位） */
int16_t Encoder_Get(void)
{
    int16_t Temp = (int16_t)TIM_GetCounter(TIM4);
    TIM_SetCounter(TIM4, 0);
    return Temp;
}

void Encoder_DeInit(void)
{
    /* 关闭计数器并复位外设寄存器 */
    TIM_Cmd(TIM4, DISABLE);
    TIM_SetCounter(TIM4, 0);
    TIM_DeInit(TIM4);

    /* 将 PB6/PB7 恢复为模拟输入（低功耗、释放引脚） */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 可选：禁用时钟（仅当确定不会被其他模块使用时） */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
}
