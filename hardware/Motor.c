#include "stm32f10x.h"
#include "Claim.h"
#include "Serial.h"
#include "PID.h"

extern int flag;
extern int count;
void Motor_Init(void)
{
    /*电机1初始化*/

        /*PWM输出*/
        /*PWMA -> PA15 TIM2_CH1*/

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //重映射PA0

    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
    // 常用配置：禁用JTAG，但保留SWD（可用于ST-LINK调试）
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitTypeDef GPIO_IS;
    GPIO_IS.GPIO_Mode = GPIO_Mode_AF_PP;            //TIM2_CH1_ETR
    GPIO_IS.GPIO_Pin = GPIO_Pin_15;                  //PWMA TIM2
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
    TIM_OC1Init(TIM2, &TIM_OCIS);               //在这里启动TIM2_CH1的IC

    TIM_Cmd(TIM2, ENABLE);


        /*电机模式输出*/
        /*AIN1 -> PA11 AIN2 -> PA12*/
    GPIO_IS.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_IS.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;     //AIN1 AIN2
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_IS);
    GPIO_SetBits(GPIOA, GPIO_Pin_11);
    GPIO_SetBits(GPIOA, GPIO_Pin_12);                 //初始化均为高电平 制动模式


        /*编码器输入*/
        /*EncoderA -> PA6 TIM3_CH1 EncoderB -> PA7 TIM3_CH2*/
        /*同时TIM3每10ms产生中断更新转速*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    GPIO_IS.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_IS.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;       //电机1 编码器A B TIM3
    GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_IS);

    TIM_TBIS.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TBIS.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TBIS.TIM_Period = 65536-1;
    TIM_TBIS.TIM_Prescaler = 1 - 1;
    TIM_TBIS.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TBIS);

    TIM_ICInitTypeDef TIM_ICIS;
    TIM_ICStructInit(&TIM_ICIS);

    TIM_ICIS.TIM_Channel = TIM_Channel_1;
    TIM_ICIS.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM3, &TIM_ICIS);
    TIM_ICIS.TIM_Channel = TIM_Channel_2;
    TIM_ICIS.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM3, &TIM_ICIS);

    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_Cmd(TIM3, ENABLE);

}

void TIM1_Init(void)                //定时中断 20ms
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 1. 使能TIM1和GPIO时钟（若使用复用功能引脚）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // 2. 配置定时器基本参数
    // 目标：20ms定时。假设系统时钟72MHz，TIM1挂载在APB2（通常与系统时钟同频）
    // 计算公式：定时时间 = (Prescaler + 1) * (Period + 1) / TIM_Clock
    // 设置预分频器为7199，则计数器时钟频率 = 72MHz / (7199 + 1) = 10kHz（周期0.1ms）
    // 设置自动重载值ARR为199，则定时周期 = (199 + 1) * 0.1ms = 20ms
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7199;          // 预分频值[2,5](@ref)
    TIM_TimeBaseInitStruct.TIM_Period = 199;              // 自动重载值ARR[2,5](@ref)
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式[5](@ref)
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;     // 时钟分频[5](@ref)
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;     // 重复计数器（高级定时器特有）[5](@ref)
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

    // 3. 使能更新中断
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);            // 使能更新中断[4,5](@ref)

    // 4. 配置NVIC（嵌套向量中断控制器）
    NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;       // TIM1更新中断通道[4](@ref)
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;       // 子优先级
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 5. 启动定时器
    TIM_Cmd(TIM1, ENABLE);                                 // 启动TIM1[4,5](@ref)
}
/**
  * 函    数：获取编码器的增量值
  * 参    数：无
  * 返 回 值：自上此调用此函数后，编码器的增量值
  */
int16_t Motor1_getSpeed(void)
{
	/*使用Temp变量作为中继，目的是返回CNT后将其清零*/
	int16_t Temp;
	Temp = TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3, 0);
	return Temp;
}
// 6. 编写中断服务函数
void TIM1_UP_IRQHandler(void)                             // TIM1更新中断服务函数[4,5](@ref)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)    // 检查更新中断标志
    {

        Motor1_Speed = Motor1_getSpeed();   				//每隔固定时间段读取一次编码器计数增量值，即为速度值
		//TIM_ClearITPendingBit(TIM3, TIM_IT_Update);			//清除TIM2更新事件的中断标志位
															//中断标志位必须清除
															//否则中断将连续不断地触发，导致主程序卡死
        float temp = (float) Motor1_Speed;
        Serial_SendJustFloat(&temp, 1);
        PIDControl();
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);       // 清除中断标志位[4,5](@ref)
    }
}



/**
  * 函    数：TIM3中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
void TIM3_IRQHandler(void)
{
	//if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)		//判断是否是TIM2的更新事件触发的中断
	//{        flag ++;

        //if(count == 22){

        //count = 0;
        //Motor1_Speed = Motor1_getSpeed();   				//每隔固定时间段读取一次编码器计数增量值，即为速度值
		//TIM_ClearITPendingBit(TIM3, TIM_IT_Update);			//清除TIM2更新事件的中断标志位
															////中断标志位必须清除
															////否则中断将连续不断地触发，导致主程序卡死
        //}else if(count < 22) count++;
	//}
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
    Motor1_Mode = Mode;
}
/**
  * 函    数：直流电机设置PWM
  * 参    数：PWM 要设置的PWM值，范围：-100~100（负数为反转）
  * 返 回 值：无
  */
void Motor_SetPWM(int8_t PWM)
{
	if (PWM >= 0)							//如果设置正转的PWM
	{
		//GPIO_ResetBits(GPIOB, GPIO_Pin_12);	//PB12置低电平
		//GPIO_SetBits(GPIOB, GPIO_Pin_13);	//PB13置高电平
        Motor_SetMode(1, Motor_Mode_frd_rotation);
		Motor1_SetCompare1(PWM);				//设置PWM占空比
	}
	else									//否则，即设置反转的速度值
	{
		//GPIO_SetBits(GPIOB, GPIO_Pin_12);	//PB12置高电平
		//GPIO_ResetBits(GPIOB, GPIO_Pin_13);	//PB13置低电平
        Motor_SetMode(1, Motor_Mode_rvs_rotation);
		Motor1_SetCompare1(-PWM);				//设置PWM占空比
	}
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


