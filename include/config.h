#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

namespace LOONGSON
{
    class Config
    {
        public:
            typedef struct
            {
                
            }CONFIG;
            
            Config(std::string configFilePath);
            void readJSON();
            void writeJSON(CONFIG config);
        private:
            std::string _configFilePath;
            nlohmann::json _configJSON;
    };
}

#endif