#include "ConfigManager.h"
#include <fstream>
#include <iostream>

bool ConfigManager::loadConfig(const std::string& filename, FractalParams& params)
{
    try {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        nlohmann::json j;
        file >> j;
        params = j.get<FractalParams>();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::saveConfig(const std::string& filename, const FractalParams& params)
{
    try {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        nlohmann::json j = params;
        file << j.dump(4);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}
