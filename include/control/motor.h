#ifndef MOTOR_H
#define MOTOR_H

#include"base.h"

#define GPIO_MUX_BASE_ADDR 0x16000490
#define GTIM_BASE_ADDR 0x16119000
#define GTIM_CR1_OFFSET 0x00
#define GTIM_CR2_OFFSET 0x04
#define GTIM_SMCR_OFFSET 0x08
#define GTIM_DIER_OFFSET 0x0C
#define GTIM_SR_OFFSET 0x10
#define GTIM_EGR_OFFSET 0x14
#define GTIM_CCMR1_OFFSET 0x18
#define GTIM_CCMR2_OFFSET 0x1C
#define GTIM_CCER_OFFSET 0x20
#define GTIM_CNT_OFFSET 0x24
#define GTIM_PSC_OFFSET 0x28
#define GTIM_ARR_OFFSET 0x2C
#define GTIM_CCR1_OFFSET 0x34
#define GTIM_CCR2_OFFSET 0x38
#define GTIM_CCR3_OFFSET 0x3C
#define GTIM_CCR4_OFFSET 0x40
#define GTIM_INSTA_OFFSET 0x50

extern void init_gpio_motor_dir();
extern void export_gpio(uint32 gpio_num);
extern void set_gpio_dir(const char* gpio_num_i,const char* model);
extern void set_gpio_value(const char* gpio_num_i,const char* potential);

extern uint PWM_MOTOR;
extern int MOTOR_PID;

#define OX  (50/ 3000.0)  //标度变换
extern float Disf;

void set_duty_cycle(const char* pwmchip, uint32 duty_cycle_10ns);

extern void set_gpio_value(const char* gpio_num_i, const char* potential);

class PWM_GTIM
{
public:
    PWM_GTIM(int gpio, int mux, int chNum_, int period_, int duty_cycle_);
    ~PWM_GTIM(void);

    void enable(void);
    void disable(void);
    void setPeriod(unsigned int period_10ns_);
    void setDutyCycle(unsigned int duty_cycle_10ns_);
    uint32_t period_10ns, duty_cycle_10ns;

private:
    uint32_t chNum;
    void *ccmr_buffer[2];
    void *ccer_buffer;
    void *period_buffer;
    void *duty_cycle_buffer;
    void *cnt_buffer;
};

extern void Motor_Control(PWM_GTIM L_Motor, PWM_GTIM R_Motor,int L_motor_duty, int R_motor_duty);

#endif