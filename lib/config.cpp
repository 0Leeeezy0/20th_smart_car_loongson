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

    auto KPs = _configJSON["SERVO_KP"];
    int numKP = 0;
    for (const auto& KP : KPs) 
    {
        SERVO_KP[numKP] = (float)KP;
        numKP++;
    }
    auto KIs = _configJSON["SERVO_KI"];
    int numKI = 0;
    for (const auto& KI : KIs) 
    {
        SERVO_KI[numKI] = (float)KI;
        numKI++;
    }
    auto KDs = _configJSON["SERVO_KD"];
    int numKD = 0;
    for (const auto& KD : KDs) 
    {
        SERVO_KD[numKD] = (float)KD;
        numKD++;
    }
}

void LOONGSON::Config::writeJSON(LOONGSON::Config::CONFIG config)
{
    std::ofstream _configFile(_configFilePath);
}