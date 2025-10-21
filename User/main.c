#ifndef __STM32_
#define __STM32_
#include "stm32f10x.h"                  // Device header
#endif
#include "Delay.h"
#include "oled.h"
uint8_t KeyNum;

int main()
{
	
	OLED_Init();
    OLED_ShowString(1,1,"to c or not to c");
	
}

