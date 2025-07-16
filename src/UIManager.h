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

    using FileActionCallback = std::function<void(const std::string&)>;
    void setFileActionCallback(FileActionCallback callback)
    {
        m_fileActionCallback = callback;
    }

private:
    void drawMainWindow();
    void drawFileDialog();
    void drawPerformanceWindow(float deltaTime);
    void drawPresetsPanel();

    FractalParams& m_fractalParams;
    ConfigManager m_configManager;
    bool m_showFileDialog = false;
    bool m_isSaving = false;
    std::string m_currentFile = "config.json";

    bool m_showPerformance = true;
    bool m_showPresets = false;
    FileActionCallback m_fileActionCallback;
};
