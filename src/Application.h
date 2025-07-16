#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Renderer.h"
#include "UIManager.h"

class Application
{
public:
    Application();
    ~Application();

    void run();

private:
    void processInput();
    void update();
    void render();
    void setupCallbacks();

    GLFWwindow* m_window = nullptr;
    Camera m_camera;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<UIManager> m_uiManager;
    bool m_running = true;
    double m_lastFrameTime = 0.0;
    int m_width = 1920;
    int m_height = 1080;
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
};
