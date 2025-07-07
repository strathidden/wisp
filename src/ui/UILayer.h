#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

class Application;
class IFractal;

class UILayer
{
public:
    void initialize(GLFWwindow* window);
    void shutdown();

    void beginFrame();
    void endFrame();

    void draw(Application& app);

private:
    void drawMainMenu(Application& app);
    void drawFractalControls(IFractal* fractal);
    void drawFractalSelector(Application& app);

    char m_savePath[256] = "config.json";
    char m_loadPath[256] = "config.json";
};
