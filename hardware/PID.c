#include "stm32f10x.h"
#include "Claim.h"
#include "Motor.h"
// 在文件开头，定义积分分离阈值（根据实际系统调整此值）
#define INTEGRAL_SEPARATION_THRESHOLD 100  

void PIDControl(void)
{
    			/*获取实际速度值*/
			Actual = Motor1_Speed;
			
			/*获取本次误差、上次误差和上上次误差*/
    Error2 = Error1;            //获取上上次误差
    Error1 = Error0;            //获取上次误差
    Error0 = Target - Actual;    //获取本次误差
			
    /*积分分离PID计算*/
    float deltaP = Kp * (Error0 - Error1);  //比例项
    float deltaD = Kd * (Error0 - 2 * Error1 + Error2); //微分项
    float deltaI = 0;  //积分项，默认设为0（分离状态）
    
    //积分分离判断：只有当误差在阈值内时才加入积分作用[1,2](@ref)
    if((Error0 >= 0 ? Error0 : -Error0) <= INTEGRAL_SEPARATION_THRESHOLD) {
        deltaI = Ki * Error0;  //引入积分控制
    }
    
    //计算总输出增量
    Out += deltaP + deltaI + deltaD;
			
			/*输出限幅*/
    if (Out > 100) {Out = 100;}     //限制输出值最大为200
    if (Out < -100) {Out = -100;}   //限制输出值最小为-200
    if(Target == 0 && Actual == 0) Out = 0;
			
			/*执行控制*/
			Motor_SetPWM(Out);
}
