#include "steer.h"

PID_Datatypedef SteerPIDdata;    //舵机的PID参数

float iError,//当前误差   原来是int类型 5.23更改为float
      SteerErr,
      SteerErr_pre;
int PWM;

void init_pwm()
{
    system("echo ""0"" > /sys/class/pwm/pwmchip0/export");
    //system("cat /sys/class/pwm/pwmchip0/pwm0/polarity");
    system("echo ""inversed"" > /sys/class/pwm/pwmchip0/pwm0/polarity");
    //system("cat /sys/class/pwm/pwmchip0/pwm0/polarity");
    system("echo ""20000000"" > /sys/class/pwm/pwmchip0/pwm0/period");
    system("echo ""0"" > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
    system("echo ""1"" > /sys/class/pwm/pwmchip0/pwm0/enable");
}

void export_pwm(const char* pwmchip)
{
    std::string command = "echo 0 > /sys/class/pwm/" + std::string(pwmchip) + "/export";
    system(command.c_str());
}


void set_period(const char* pwmchip, uint32 period_10ns)
{
    std::string command = "echo " + std::string(std::to_string(period_10ns)) + " > /sys/class/pwm/" + std::string(pwmchip) + "/pwm0/period";
    system(command.c_str());
}


void set_polarity(const char* pwmchip, const char* polarity_i)
{
    std::string command = "echo " + std::string(polarity_i) + " > /sys/class/pwm/" + std::string(pwmchip) + "/pwm0/polarity";
    system(command.c_str());
}

void enable_pwm(const char* pwmchip)
{
    std::string command = "echo 1 > /sys/class/pwm/" + std::string(pwmchip) + "/pwm0/enable";
    system(command.c_str());
}


void set_duty_cycle(const char* pwmchip, uint32 duty_cycle_10ns)
{
    std::string command = "echo " + std::string(std::to_string(duty_cycle_10ns)) + " > /sys/class/pwm/" + std::string(pwmchip) + "/pwm0/duty_cycle";
    system(command.c_str());
}


void SteerControl(int dir, int angle)
{
    int real_angle = dir*angle;
    uint32 h_time = 1550000+(real_angle*2000000/1100);

    set_duty_cycle("pwmchip0",h_time);
}

void SteerControl_real(uint32 h_time)
{
    
}