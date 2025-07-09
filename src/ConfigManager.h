#pragma once
#include "Fractal.h"
#include <string>

class ConfigManager
{
public:
    bool loadConfig(const std::string& filename, FractalParams& params);
    bool saveConfig(const std::string& filename, const FractalParams& params);
};
