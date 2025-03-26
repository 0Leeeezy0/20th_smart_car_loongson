#include "control.h"
#include "common.h"
#include "register.h"

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>

/************************************************************* PWM ***************************************************************/

LOONGSON::PWM::PWM(const std::string& chip, int channel) : chip_(chip), basePath_("/sys/class/pwm/" + chip + "/pwm" + std::to_string(channel)), channel_(channel)
{
    disable();
    unexportPWM();
    exportPWM(chip, channel);
}

LOONGSON::PWM::~PWM() 
{
    disable();
    unexportPWM();
}

void LOONGSON::PWM::setPeriod(unsigned int period) 
{
    writeToFile(basePath_ + "/period", std::to_string(period));
}

void LOONGSON::PWM::setDutyCycle(unsigned int duty_cycle) 
{
    writeToFile(basePath_ + "/duty_cycle", std::to_string(duty_cycle));
}

void LOONGSON::PWM::enable() 
{
    writeToFile(basePath_ + "/enable", "1");
}

void LOONGSON::PWM::disable() 
{
    writeToFile(basePath_ + "/enable", "0");
}

void LOONGSON::PWM::exportPWM(const std::string& chip, int channel) 
{
    std::ofstream exportFile("/sys/class/pwm/" + chip + "/export");
    exportFile << channel;
}

void LOONGSON::PWM::unexportPWM() 
{
    std::ofstream unexportFile("/sys/class/pwm/" + chip_ + "/unexport");
    unexportFile << channel_;
}

void LOONGSON::PWM::writeToFile(const std::string& path, const std::string& value) 
{
    std::ofstream file(path);
    if (file.is_open()) {
        file << value;
    } else {
        std::cerr << "Failed to open " << path << std::endl;
    }
}

/************************************************************* GTIM ***************************************************************/

LOONGSON::GTIM::GTIM(int gpio, int mux, int chNum_, int period_10ns_, int duty_cycle_10ns_) : period_10ns(period_10ns_), duty_cycle_10ns(duty_cycle_10ns_), chNum(chNum_ - 1)
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

    // 清除chNum的输出极性
    REG_WRITE(ccer_buffer, REG_READ(ccer_buffer) & ~(0x1 << (chNum * 4 + 1))); // 1为反相
    // 配置chNum的输出极性
    REG_WRITE(ccer_buffer, REG_READ(ccer_buffer) | (0x1 << (chNum * 4 + 1))); // 1为反相

    REG_WRITE(period_buffer, period_10ns);
    REG_WRITE(duty_cycle_buffer, duty_cycle_10ns);

    REG_WRITE(cnt_buffer, 0);
    printf("Registers mapped successfully\n");
}

LOONGSON::GTIM::~GTIM(void)
{
    munmap(ccmr_buffer[0], PAGE_SIZE);
    munmap(ccmr_buffer[1], PAGE_SIZE);
    munmap(period_buffer, PAGE_SIZE);
    munmap(duty_cycle_buffer, PAGE_SIZE);
    munmap(ccer_buffer, PAGE_SIZE);
    munmap(cnt_buffer, PAGE_SIZE);
}

void LOONGSON::GTIM::enable(void)
{
    REG_WRITE(ccer_buffer, REG_READ(ccer_buffer) | (0x1 << (chNum * 4 + 0)));
}

void LOONGSON::GTIM::disable(void)
{
    REG_WRITE(ccer_buffer, REG_READ(ccer_buffer) & ~(0x1 << (chNum * 4 + 0)));
}

// 设置周期（以10纳秒为单位）
void LOONGSON::GTIM::setPeriod(unsigned int period_10ns_)
{
    period_10ns = period_10ns_;
    REG_WRITE(period_buffer, period_10ns);
}

// 设置低电平时间（以10纳秒为单位）
void LOONGSON::GTIM::setDutyCycle(unsigned int duty_cycle_10ns_)
{
    duty_cycle_10ns = duty_cycle_10ns_;
    REG_WRITE(duty_cycle_buffer, duty_cycle_10ns);

    // REG_WRITE(cnt_buffer, 0);
}

/************************************************************* GPIO ***************************************************************/

LOONGSON::GPIO::GPIO(int pin) : pin_(pin) 
{
    unexportGPIO();
    exportGPIO();
}

LOONGSON::GPIO::~GPIO() 
{
    unexportGPIO();
}

void LOONGSON::GPIO::setDirection(const std::string& direction)
{
    writeToFile("/sys/class/gpio/gpio" + std::to_string(pin_) + "/direction", direction);
}

void LOONGSON::GPIO::setValue(int value) 
{
    writeToFile("/sys/class/gpio/gpio" + std::to_string(pin_) + "/value", std::to_string(value));
}

int LOONGSON::GPIO::getValue() 
{
    std::ifstream valueFile("/sys/class/gpio/gpio" + std::to_string(pin_) + "/value");
    int value;
    valueFile >> value;
    return value;
}

void LOONGSON::GPIO::exportGPIO() 
{
    std::ofstream exportFile("/sys/class/gpio/export");
    exportFile << pin_;
}

void LOONGSON::GPIO::unexportGPIO() 
{
    std::ofstream unexportFile("/sys/class/gpio/unexport");
    unexportFile << pin_;
}

void LOONGSON::GPIO::writeToFile(const std::string& path, const std::string& value) 
{
    std::ofstream file(path);
    if (file.is_open()) {
        file << value;
    } else {
        std::cerr << "Failed to open " << path << std::endl;
    }
}

/************************************************************* PID ***************************************************************/

LOONGSON::PID::PID(LOONGSON::PID::PID_KIND pidKind) :_pidKind(pidKind)
{
}

float LOONGSON::PID::getValue(float target, float feedback)
{
    _nowErr = target-feedback;

    switch(_pidKind)
    {
        case INCREMENTAL:
        {
            // 增量式 p
            _delta += kp*(_nowErr-_lastErr);
            // 增量式 i
            _delta += ki*_nowErr;
            // 增量式 d
            _delta += kd*(_nowErr-2*_lastErr+_lastLastErr);
                        
            // 更新参数
            _value += _delta;
            _lastErr = _nowErr;
            _lastLastErr = _lastErr;
            _delta = 0;
            
            // 输出限幅
            if(_value > outputLimit)
            {
                _value = outputLimit;
            }
            if(_value < -outputLimit)
            {
                _value = -outputLimit;
            }
            break;
        }
        case POSITIONAL:
        {
            _value = 0;
            // 增量式 p
            _value += kp*_nowErr;
            // 增量式 i
            _value += ki*_sigmaErr;
            // 增量式 d
            _value += kd*(_nowErr-_lastErr);
                        
            // 更新参数
            _sigmaErr += _nowErr;
            _lastErr = _nowErr;
            
            // 输出限幅
            if(_value > outputLimit)
            {
                _value = outputLimit;
            }
            if(_value < -outputLimit)
            {
                _value = -outputLimit;
            }
            // 积分限幅
            if(_sigmaErr > iLimit)
            {
                _sigmaErr = iLimit;
            }
            if(_sigmaErr < -iLimit)
            {
                _sigmaErr = -iLimit;
            }
            break;
        }
    }
    return _value;
}

/************************************************************* 编码器 ***************************************************************/

// pwmNum为STEP引脚连接的PWM通道编号，gpioNum为DIR引脚所接的GPIO编号
LOONGSON::Encoder::Encoder(int pwmNum, int gpioNum) : _baseAddr(PWM_BASE_ADDR + pwmNum * PWM_OFFSET), directionGPIO(gpioNum)
{
    directionGPIO.setDirection("in");

    control_buffer = map_register(_baseAddr + CONTROL_REG_OFFSET, PAGE_SIZE);
    low_buffer = map_register(_baseAddr + LOW_BUFFER_OFFSET, PAGE_SIZE);
    full_buffer = map_register(_baseAddr + FULL_BUFFER_OFFSET, PAGE_SIZE);

    printf("Registers mapped successfully\n");

    initPWM();
}

// 析构函数
LOONGSON::Encoder::~Encoder(void)
{
    // directionGPIO.~GPIO();
    munmap(control_buffer, PAGE_SIZE);
    munmap(low_buffer, PAGE_SIZE);
    munmap(full_buffer, PAGE_SIZE);
}

// 初始化PWM控制器为计数模式
void LOONGSON::Encoder::initPWM(void)
{
    uint32_t control_reg = 0;

    control_reg |= CNTR_ENABLE_BIT;
    control_reg |= MEASURE_PULSE_BIT;
    control_reg |= INT_ENABLE_BIT;

    REG_WRITE(control_buffer, control_reg);
}

// 清空计数器
void LOONGSON::Encoder::resetCounter(void)
{
    uint32_t control_reg = REG_READ(control_buffer);
    control_reg |= COUNTER_RESET_BIT;
    REG_WRITE(control_buffer, control_reg);
}

// 返回编码器的RPS
double LOONGSON::Encoder::pulseCounterUpdate(void)
{
    double value = 100000000.0 / REG_READ(full_buffer) / ENCODER_PPR * (directionGPIO.getValue() * 2 - 1);
    return value;
}

/************************************************************* 电机 ***************************************************************/

LOONGSON::Motor::Motor(const char* pwmchip, int gpioNum, bool reverse) : _reverse(reverse)
{
    GPIO_motor = new GPIO(gpioNum);
    GPIO_motor->setDirection("out");
    PWM_motor = new PWM(pwmchip, 0);
    PWM_motor->setPeriod(200000);
    PWM_motor->enable();
}

LOONGSON::Motor::~Motor()
{
    PWM_motor->~PWM();
    GPIO_motor->~GPIO();
}

void LOONGSON::Motor::setMotorRotate(int dir, unsigned int duty)
{
    if(duty >= 0 && duty <= 200000)
    {
        PWM_motor->setDutyCycle(duty);
    }
    if(_reverse == false)
    {
        switch(dir)
        {
            case 1:{ GPIO_motor->setValue(1); break; }
            case 0:{ GPIO_motor->setValue(0); break; }
        }
    }
    else
    {
        switch(dir)
        {
            case 1:{ GPIO_motor->setValue(0); break; }
            case 0:{ GPIO_motor->setValue(1); break; }
        }
    }
}

/************************************************************* 舵机 ***************************************************************/

LOONGSON::Servo::Servo(int pwmchip)
{
    PWM_servo = new GTIM(pwmchip, 0b11, 2, 2000000, 0);
    PWM_servo->enable();
}

LOONGSON::Servo::~Servo()
{
    PWM_servo->~GTIM();
}

void LOONGSON::Servo::setServoRotate(int angle)
{
    int lowTime = SERVO_MID_DUTY+(angle*200000/180);
    if(lowTime >= SERVO_R_LIMIT_DUTY+SERVO_MID_DUTY)
        lowTime = SERVO_MID_DUTY+SERVO_R_LIMIT_DUTY;
    else if(lowTime <= -SERVO_L_LIMIT_DUTY+SERVO_MID_DUTY)
        lowTime = SERVO_MID_DUTY-SERVO_L_LIMIT_DUTY;
    PWM_servo->setDutyCycle(lowTime);
}