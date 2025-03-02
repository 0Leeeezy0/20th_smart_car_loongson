#include "motor.h"
#include "register.h"

uint PWM_MOTOR;
int MOTOR_PID;

float Disf;

void init_gpio_motor_dir()
{
    system("echo 73 > /sys/class/gpio/export");
    system("echo 72 > /sys/class/gpio/export");
    system("echo ""out"" > /sys/class/gpio/gpio73/direction");
    //system("cat /sys/class/gpio/gpio73/direction");
    system("echo ""out"" > /sys/class/gpio/gpio72/direction");
    system("echo ""1"" > /sys/class/gpio/gpio73/value");
    //system("cat /sys/class/gpio/gpio73/value");
    system("echo ""0"" > /sys/class/gpio/gpio72/value");
    
}

void export_gpio(uint32 gpio_num)
{
    std::string command = "echo " + std::string(std::to_string(gpio_num)) + " > /sys/class/gpio/export";
    system(command.c_str());
}


void set_gpio_dir(const char* gpio_num_i, const char* model)
{
    std::string command = "echo " + std::string(model) + " > /sys/class/gpio/" + std::string(gpio_num_i) + "/direction";
    system(command.c_str());
}


void set_gpio_value(const char* gpio_num_i, const char* potential)
{
    std::string command = "echo " + std::string(potential) + " > /sys/class/gpio/" + std::string(gpio_num_i) + "/value";
    system(command.c_str());
}

PWM_GTIM::PWM_GTIM(int gpio, int mux, int chNum_, int period_10ns_, int duty_cycle_10ns_)
    : period_10ns(period_10ns_), duty_cycle_10ns(duty_cycle_10ns_), chNum(chNum_ - 1)
{
    { // 配置功能复用
        void *gpio_mux_buffer = map_register(GPIO_MUX_BASE_ADDR + (gpio / 16) * 0x04, PAGE_SIZE);
        REG_WRITE(gpio_mux_buffer, (REG_READ(gpio_mux_buffer) & ~(0b11 << (gpio % 16 * 2))) | (mux << (gpio % 16 * 2)));
    }

    // 初始化所有寄存器
    REG_WRITE(map_register(GTIM_BASE_ADDR + GTIM_EGR_OFFSET, PAGE_SIZE), 0x01);

    // 启动计数器
    REG_WRITE(map_register(GTIM_BASE_ADDR + GTIM_CR1_OFFSET, PAGE_SIZE), 0x01);

    period_buffer = map_register(GTIM_BASE_ADDR + GTIM_ARR_OFFSET, PAGE_SIZE);
    duty_cycle_buffer = map_register(GTIM_BASE_ADDR + GTIM_CCR1_OFFSET + chNum * 0x04, PAGE_SIZE);
    ccmr_buffer[0] = map_register(GTIM_BASE_ADDR + GTIM_CCMR1_OFFSET, PAGE_SIZE);
    ccmr_buffer[1] = map_register(GTIM_BASE_ADDR + GTIM_CCMR2_OFFSET, PAGE_SIZE);
    ccer_buffer = map_register(GTIM_BASE_ADDR + GTIM_CCER_OFFSET, PAGE_SIZE);
    cnt_buffer = map_register(GTIM_BASE_ADDR + GTIM_CNT_OFFSET, PAGE_SIZE);

    // 清除chNum的PWM模式
    REG_WRITE(ccmr_buffer[chNum / 2], REG_READ(ccmr_buffer[chNum / 2]) & ~(0x7 << (chNum % 2 * 8 + 4)));
    // 配置chNum的PWM模式 0x6为模式1 0x7为模式2
    REG_WRITE(ccmr_buffer[chNum / 2], REG_READ(ccmr_buffer[chNum / 2]) | (0x7 << (chNum % 2 * 8 + 4)));

    // 清除chNum的输出极性// 设置周期（以10纳秒为单位）
    // 配置chNum的输出极性
    REG_WRITE(ccer_buffer, REG_READ(ccer_buffer) | (0x1 << (chNum * 4 + 1))); // 1为反相

    REG_WRITE(period_buffer, period_10ns);
    REG_WRITE(duty_cycle_buffer, duty_cycle_10ns);

    REG_WRITE(cnt_buffer, 0);
    printf("Registers mapped successfully\n");
}

PWM_GTIM::~PWM_GTIM(void)
{
    munmap(ccmr_buffer[0], PAGE_SIZE);
    munmap(ccmr_buffer[1], PAGE_SIZE);
    munmap(period_buffer, PAGE_SIZE);
    munmap(duty_cycle_buffer, PAGE_SIZE);
    munmap(ccer_buffer, PAGE_SIZE);
    munmap(cnt_buffer, PAGE_SIZE);
}

void PWM_GTIM::enable(void)
{
    REG_WRITE(ccer_buffer, REG_READ(ccer_buffer) | (0x1 << (chNum * 4 + 0)));
}

void PWM_GTIM::disable(void)
{
    REG_WRITE(ccer_buffer, REG_READ(ccer_buffer) & ~(0x1 << (chNum * 4 + 0)));
}


void PWM_GTIM::setPeriod(unsigned int period_10ns_)
{
    period_10ns = period_10ns_;
    REG_WRITE(period_buffer, period_10ns);

    REG_WRITE(cnt_buffer, 0);
}


void PWM_GTIM::setDutyCycle(unsigned int duty_cycle_10ns_)
{
    duty_cycle_10ns = duty_cycle_10ns_;
    REG_WRITE(duty_cycle_buffer, duty_cycle_10ns);

    REG_WRITE(cnt_buffer, 0);
}

void Motor_Control(PWM_GTIM L_Motor, PWM_GTIM R_Motor,int L_motor_duty, int R_motor_duty)
{
    L_Motor.setDutyCycle(L_motor_duty);//test_l.disable();
    R_Motor.setDutyCycle(R_motor_duty);//test_l.disable();
}

