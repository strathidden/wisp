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
    void drawPerformanceWindow(float deltaTime);
    void drawPresetsPanel();
    void drawColorPicker(const std::string& label, glm::vec3& color);

    FractalParams& m_fractalParams;
    ConfigManager m_configManager;
    bool m_showFileDialog = false;
    bool m_isSaving = false;
    std::string m_currentFile = "config.json";

    bool m_showPerformance = false;
    bool m_showPresets = false;
    bool m_showColorPicker1 = false;
    bool m_showColorPicker2 = false;
};
