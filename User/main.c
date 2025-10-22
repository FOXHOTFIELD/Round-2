#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "oled.h"
#include "Claim.h"
#include "Motor.h"

enum Motor_Mode Motor1_Mode = Motor_Mode_break;
enum Motor_Mode Motor2_Mode = Motor_Mode_break;

int main()
{
	
	OLED_Init();
    OLED_ShowNum(1,1,(Motor1_Mode == Motor_Mode_rvs_rotation ? 1 : 2), 1);
    Motor_Init();
    Motor_SetMode(1,     Motor_Mode_rvs_rotation);
    Motor1_SetPrescaler(72-1);
    Motor1_SetCompare1(50);

    while (1)
    {
    }
    
}

