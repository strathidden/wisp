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
        addRecentFile(filename);
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
        addRecentFile(filename);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

void ConfigManager::addRecentFile(const std::string& filename)
{
    auto it = std::find(m_recentFiles.begin(), m_recentFiles.end(), filename);
    if (it != m_recentFiles.end())
    {
        m_recentFiles.erase(it);
    }

    m_recentFiles.insert(m_recentFiles.begin(), filename);

    if (m_recentFiles.size() > m_maxRecentFiles)
    {
        m_recentFiles.pop_back();
    }
}

std::vector<std::string> ConfigManager::getRecentFiles(size_t maxCount) const
{
    if (maxCount < m_recentFiles.size())
    {
        return std::vector<std::string>(m_recentFiles.begin(), m_recentFiles.begin() + maxCount);
    }
    return m_recentFiles;
}
