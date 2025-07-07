#pragma once

#include "../ui/UILayer.h"
#include "../graphics/Camera.h"
#include "../graphics/Renderer.h"
#include "../graphics/Shader.h"
#include "../fractals/FractalRegistry.h"
#include <memory>

class Application
{
public:
    Application();
    ~Application();

    void run();

    FractalRegistry m_fractalRegistry;
    std::unique_ptr<IFractal> m_currentFractal;

    Camera m_camera;
    Shader m_shader;
    Renderer m_renderer;
    UILayer m_ui;

    GLFWwindow* m_window = nullptr;
private:
    void handleInput();
    void updateCamera();

    int m_width = 1920;
    int m_height = 1080;
    float m_aspect = 16.0f / 9.0f;

};
