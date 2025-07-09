#pragma once
#include "Fractal.h"
#include "ConfigManager.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

class UIManager
{
public:
    UIManager(GLFWwindow* window, FractalParams& params);
    ~UIManager();

    void render(float deltaTime);
    bool wantCaptureMouse() const;
    bool wantCaptureKeyboard() const;
    FractalParams& getFractalParams() { return m_fractalParams; }

private:
    void drawMainWindow();
    void drawFileDialog();

    FractalParams& m_fractalParams;
    ConfigManager m_configManager;
    std::string m_currentFile = "config.json";
    std::vector<std::string> m_recentFiles;
    bool m_showFileDialog = false;
    bool m_isSaving = false;
};
