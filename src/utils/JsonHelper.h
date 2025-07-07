#pragma once

#include "../graphics/Camera.h"
#include "../fractals/IFractal.h"
#include <nlohmann/json.hpp>
#include <string>

class JsonHelper
{
public:
    static void saveConfig(const std::string& path, const IFractal& fractal, const Camera& camera);
    static void loadConfig(const std::string& path, IFractal& fractal, Camera& camera);
};
