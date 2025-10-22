#ifndef __CLAIM_H
#define __CLAIM_H

enum Motor_Mode{
    Motor_Mode_break,
    Motor_Mode_frd_rotation,
    Motor_Mode_rvs_rotation,
    Motor_Mode_stop
};

extern enum Motor_Mode Motor1_Mode;
extern enum Motor_Mode Motor2_Mode;



#endif
