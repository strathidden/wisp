#pragma once
#include "Fractal.h"
#include <string>

class ConfigManager
{
public:
    bool loadConfig(const std::string& filename, FractalParams& params);
    bool saveConfig(const std::string& filename, const FractalParams& params);
    std::vector<std::string> getRecentFiles(size_t maxCount = 5) const;
    void addRecentFile(const std::string& filename);

private:
    std::vector<std::string> m_recentFiles;
    const size_t m_maxRecentFiles = 5;
};
