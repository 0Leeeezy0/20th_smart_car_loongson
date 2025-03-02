#ifndef STEER_H
#define STEER_H

#include"base.h"

#define steer_middle  845
#define steer_right   760            //舵机右打 512  85   342  1865   1600 623
#define steer_left    920            //舵机左打 685 512  600 1000  2325  2300 793

#define LimitLeft(Left)    (Left = ((Left > steer_left) ? steer_left : Left))
#define LimitRight(Right)  (Right = ((Right < steer_right) ? steer_right : Right))

extern void init_pwm(void);
void export_pwm(const char* pwmchip);
void set_polarity(const char* pwmchip,const char* polarity_i);
void set_period(const char* pwmchip,uint32 period_10ns);
void set_duty_cycle(const char* pwmchip,uint32 duty_cycle_10ns);
void enable_pwm(const char* pwmchip);


typedef struct 
{
  float P;
  float I;
  float D;
  int LastError;  // Error[-1]
  int PrevError;  // Error[-2]
  int EC;
  float Kdin;   //入弯D
  float Kdout;  //出弯D
} PID_Datatypedef;

extern void SteerControl(int dir, int angle);

#endif