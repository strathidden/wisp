#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <functional>
#include <vector>
#include <unordered_map>

class Input
{
public:
    using KeyCallback = std::function<void(int key, int action)>;
    using MouseButtonCallback = std::function<void(int button, int action)>;
    using ScrollCallback = std::function<void(double xoffset, double yoffset)>;

    Input(GLFWwindow* window);

    void update();

    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int key) const;
    glm::dvec2 getCursorPosition() const;
    glm::dvec2 getCursorDelta() const;
    double getScrollDelta() const;

    void registerKeyCallback(KeyCallback callback);
    void registerMouseButtonCallback(MouseButtonCallback callback);
    void registerScrollCallback(ScrollCallback callback);

private:
    GLFWwindow* m_window;
    std::unordered_map<int, bool> m_keyStates;
    std::unordered_map<int, bool> m_mouseStates;
    glm::dvec2 m_cursorPos = {0, 0};
    glm::dvec2 m_lastCursorPos = {0, 0};
    double m_scrollDelta = 0;

    std::vector<KeyCallback> m_keyCallbacks;
    std::vector<MouseButtonCallback> m_mouseButtonCallbacks;
    std::vector<ScrollCallback> m_scrollCallbacks;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
