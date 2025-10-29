#include "MyHeader.h"

// 在文件开头，定义积分分离阈值（根据实际系统调整此值）
#define Outer_INTEGRAL_SEPARATION_THRESHOLD 200
#define INTEGRAL_SEPARATION_THRESHOLD 200

void PIDControl(void)
{
    int s_speed_location = 0;
    if(test == TEST_1 && ((pid_subject > 0 ? pid_subject : -pid_subject) <= 0.5))
    {
        s_speed_location = 1, pid_subject = 0;
        g_oled_show_oneplus_request = 1;
    }
    else Target = pid_subject;
    if (test == TEST_2 || s_speed_location == 1)
    {
        /*获取实际位置值*/
		OuterActual += Motor1_Speed;
        /**/
		OuterTarget = Encoder_Count;

        if(s_speed_location == 1) s_speed_location = 0, OuterTarget = 0;

		/*获取本次误差、上次误差和上上次误差*/
        OuterError2 = OuterError1;            //获取上上次误差
        OuterError1 = OuterError0;            //获取上次误差
        OuterError0 = OuterTarget - OuterActual;    //获取本次误差
			
        /*积分分离PID计算*/
        float OuterdeltaP = OuterKp * (OuterError0 - OuterError1);  //比例项
        float OuterdeltaD = OuterKd * (OuterError0 - 2 * OuterError1 + OuterError2); //微分项
        float OuterdeltaI = 0;  //积分项，默认设为0（分离状态）
    
        //积分分离判断：只有当误差在阈值内时才加入积分作用[1,2](@ref)
        if((OuterError0 >= 0 ? OuterError0 : -OuterError0) <= Outer_INTEGRAL_SEPARATION_THRESHOLD)
        {
            OuterdeltaI = OuterKi * OuterError0;  //引入积分控制
        }
    
        //计算总输出增量
        OuterOut += OuterdeltaP  + OuterdeltaD;
			
        //OuterOut += (OuterOut > 0 ? 5 : -5);
        if((OuterError0 > 0 ? OuterError0 : -OuterError0) < 7) OuterOut = 0;    //死区
    
		/*输出限幅*/
        if (OuterOut > 100) {OuterOut = 100;}     //限制输出值最大为200
        if (OuterOut < -100) {OuterOut = -100;}   //限制输出值最小为-200
        if(OuterTarget == 0 && OuterActual == 0) OuterOut = 0;

        Target = OuterOut;

    }
    	/*获取实际速度值*/
		Actual = Motor1_Speed;
			
		/*获取本次误差、上次误差和上上次误差*/
        Error2 = Error1;            //获取上上次误差
        Error1 = Error0;            //获取上次误差
        Error0 = Target - Actual;    //获取本次误差


		if((Error0 > 0 ? Error0 : -Error0) > 1)
        {
        
            /*积分分离PID计算*/
            float deltaP = Kp * (Error0 - Error1);  //比例项
            float deltaD = Kd * (Error0 - 2 * Error1 + Error2); //微分项
            float deltaI = 0;  //积分项，默认设为0（分离状态）
        
        // 积分分离判断：只有当误差在阈值内时才加入积分作用[1,2](@ref)
            if((Error0 >= 0 ? Error0 : -Error0) <= INTEGRAL_SEPARATION_THRESHOLD)
            {
                deltaI = Ki * Error0;  //引入积分控制
            }
        
            //计算总输出增量
            Out += deltaP + deltaI + deltaD;
                
            /*输出限幅*/
            if (Out > 100) {Out = 100;}     //限制输出值最大为100
            if (Out < -100) {Out = -100;}   //限制输出值最小为-100
            if(Target == 0 && Actual == 0) Out = 0;

            //if(Out == 0) Motor_SetMode(1, Motor_Mode_break);
            //else if(Out > 0) Motor_SetMode(1, Motor_Mode_frd_rotation);
            //else if (Out < 0) Motor_SetMode(1, Motor_Mode_rvs_rotation)
       } 
	/*执行控制*/
	Motor_SetPWM(Out);
}
