#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <iostream>
#include <string>
#include <cstdint>

#define SERVO_MID_DUTY  160000
#define SERVO_R_LIMIT_DUTY 15500
#define SERVO_L_LIMIT_DUTY 13500

namespace LOONGSON
{
    class PWM
    {
        public:
            PWM(const std::string& chip, int channel);
            ~PWM();
            void setPeriod(unsigned int period);
            void setDutyCycle(unsigned int duty_cycle);
            void enable();
            void disable();
        private:
            std::string chip_;
            std::string basePath_;
            int channel_;
            void exportPWM(const std::string& chip, int channel);
            void unexportPWM();
            void writeToFile(const std::string& path, const std::string& value);
    };
    class GTIM
    {
        public:
            GTIM(int gpio, int mux, int chNum_, int period_, int duty_cycle_);
            ~GTIM(void);
        
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
    class GPIO
    {
        public:
            GPIO(int pin);
            ~GPIO();
            void setDirection(const std::string& direction);
            void setValue(int value);
            int getValue();
        private:
            int pin_;
            void exportGPIO();
            void unexportGPIO();
            void writeToFile(const std::string& path, const std::string& value);
    };
    class PID
    {
        public:
            typedef enum
            {
                INCREMENTAL,
                POSITIONAL
            }PID_KIND;

            float kp;
            float ki;
            float kd;
            float outputLimit;
            float iLimit;

            PID(PID_KIND pidKind);
            float getValue(float target, float feedback);
        private:
            PID_KIND _pidKind;
            float _value;
            float _delta;
            float _nowErr;
            float _lastErr;
            float _lastLastErr;
            float _sigmaErr;
    };
    class Encoder
    {
        public:
            Encoder(int pwmNum, int gpioNum);
            ~Encoder(void);

            double pulseCounterUpdate(void);

        private:
            uint32_t _baseAddr;
            GPIO directionGPIO;
            void *low_buffer;
            void *full_buffer;
            void *control_buffer;
            void initPWM(void);
            void resetCounter(void);
    };
    class Motor
    {
        public:
            Motor(const char* pwmchip, int gpioNum, bool reverse);
            ~Motor();
            void setMotorRotate(int dir, unsigned int duty);
        private:
            PWM* PWM_motor = nullptr;
            GPIO* GPIO_motor = nullptr;

            bool _reverse;
    };
    class Servo
    {
        public:
            Servo(int pwmchip);
            ~Servo();
            void setServoRotate(int angle);
        private:
            GTIM* PWM_servo = nullptr;
    };
}

#endif