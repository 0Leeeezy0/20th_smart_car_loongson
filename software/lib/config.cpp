#include "config.h"
#include "common.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "json.hpp"

LOONGSON::Config::Config(std::string configFilePath) : _configFilePath(configFilePath)
{
}

void LOONGSON::Config::readJSON()
{
    std::ifstream _configFile(_configFilePath);                
    _configFile >> _configJSON;
    _configFile.close();

    TRACK_WIDTH = (int)_configJSON["TRACK_WIDTH"];
    CONTROL_POINT = (int)_configJSON["CONTROL_POINT"];
    INFLECTION_POINT_DISTANCE = (int)_configJSON["INFLECTION_POINT_DISTANCE"];
    SKIP_POINT_DISTANCE = (int)_configJSON["SKIP_POINT_DISTANCE"];
    INFLECTION_POINT_ANGLE_MIN = (int)_configJSON["INFLECTION_POINT_ANGLE_MIN"];
    INFLECTION_POINT_ANGLE_MAX = (int)_configJSON["INFLECTION_POINT_ANGLE_MAX"];
    BEND_POINT_ANGLE_MIN = (int)_configJSON["BEND_POINT_ANGLE_MIN"];
    BEND_POINT_ANGLE_MAX = (int)_configJSON["BEND_POINT_ANGLE_MAX"];
    PREDICTION_POINT = (int)_configJSON["PREDICTION_POINT"];

    auto SERVO_KPs = _configJSON["SERVO_KP"];
    auto SERVO_numKP = 0;
    for (const auto& KP : SERVO_KPs) 
    {
        SERVO_KP[SERVO_numKP] = (float)KP;
        SERVO_numKP++;
    }
    auto SERVO_KIs = _configJSON["SERVO_KI"];
    auto SERVO_numKI = 0;
    for (const auto& KI : SERVO_KIs) 
    {
        SERVO_KI[SERVO_numKI] = (float)KI;
        SERVO_numKI++;
    }
    auto SERVO_KDs = _configJSON["SERVO_KD"];
    auto SERVO_numKD = 0;
    for (const auto& KD : SERVO_KDs) 
    {
        SERVO_KD[SERVO_numKD] = (float)KD;
        SERVO_numKD++;
    }
    
    auto MOTOR_KPs = _configJSON["MOTOR_KP"];
    auto MOTOR_numKP = 0;
    for (const auto& KP : MOTOR_KPs) 
    {
        MOTOR_KP[MOTOR_numKP] = (float)KP;
        MOTOR_numKP++;
    }
    auto MOTOR_KIs = _configJSON["MOTOR_KI"];
    auto MOTOR_numKI = 0;
    for (const auto& KI : MOTOR_KIs) 
    {
        MOTOR_KI[MOTOR_numKI] = (float)KI;
        MOTOR_numKI++;
    }
    auto MOTOR_KDs = _configJSON["MOTOR_KD"];
    auto MOTOR_numKD = 0;
    for (const auto& KD : MOTOR_KDs) 
    {
        MOTOR_KD[MOTOR_numKD] = (float)KD;
        MOTOR_numKD++;
    }
}

void LOONGSON::Config::writeJSON(LOONGSON::Config::CONFIG config)
{
    std::ofstream _configFile(_configFilePath);
}