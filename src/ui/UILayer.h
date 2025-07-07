#pragma once

#include "../graphics/Shader.h"
#include "../fractals/IFractal.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>
#include <string>

class Application;

class UILayer
{
public:
    void initialize(GLFWwindow* window);
    void shutdown();

    void beginFrame();
    void endFrame();

    void draw(Application& app, IFractal& fractal);

private:
    void drawMainMenu(Application& app, IFractal& fractal);
    void drawFractalControls(IFractal& fractal);
    void drawFractalSelector(Application& app);

    char m_savePath[256] = "config.json";
    char m_loadPath[256] = "config.json";
};
